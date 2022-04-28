#pragma once

#include <clean-core/forward.hh>

#include <texture-processor/convert.hh>
#include <texture-processor/fwd.hh>
#include <texture-processor/image_view.hh>

#include <typed-geometry/feature/vector.hh>

namespace tp
{
namespace lookup
{
// TODO: allow customized convert
template <class PixelT, class ViewT>
auto clamped(ViewT view)
{
    using ipos_t = typename ViewT::ipos_t;
    return [view, vc = ipos_t(view.extent().to_ivec() - 1)](ipos_t const& p) -> decltype(auto) {
        // clamped lookup
        if constexpr (std::is_same_v<PixelT, typename ViewT::pixel_t>)
            return view(tg::clamp(p, ipos_t(0), vc));
        else
        {
            auto src = view(tg::clamp(p, ipos_t(0), vc));
            PixelT target;
            default_converter{}(target, src);
            return target;
        }
    };
}
}

namespace interpolation
{
// TODO: allow customized mix
template <class PixelT, class ViewT, class LookupF>
auto linear(LookupF&& lookup)
{
    using pos_t = typename ViewT::pos_t;
    using ipos_t = typename ViewT::ipos_t;
    if constexpr (ViewT::dimensions == 1)
    {
        return [lookup = cc::forward<LookupF>(lookup)](pos_t const& p) -> PixelT {
            auto i0 = tg::ifloor(p);
            auto i1 = i0 + ipos_t(1);

            auto v0 = lookup(i0);
            auto v1 = lookup(i1);

            auto fx = p.x - i0.x;

            return tg::mix(v0, v1, fx);
        };
    }
    else if constexpr (ViewT::dimensions == 2)
    {
        return [lookup = cc::forward<LookupF>(lookup)](pos_t const& p) -> PixelT {
            auto i00 = tg::ifloor(p);
            auto i10 = i00 + ipos_t(1, 0);
            auto i01 = i00 + ipos_t(0, 1);
            auto i11 = i00 + ipos_t(1, 1);

            auto v00 = lookup(i00);
            auto v01 = lookup(i01);
            auto v10 = lookup(i10);
            auto v11 = lookup(i11);

            auto fx = p.x - i00.x;
            auto fy = p.y - i00.y;

            auto v0 = tg::mix(v00, v10, fx);
            auto v1 = tg::mix(v01, v11, fx);

            return tg::mix(v0, v1, fy);
        };
    }
    else if constexpr (ViewT::dimensions == 3)
    {
        static_assert(cc::always_false<ViewT>, "not implemented");
    }
    else if constexpr (ViewT::dimensions == 4)
    {
        static_assert(cc::always_false<ViewT>, "not implemented");
    }
    else
        static_assert(cc::always_false<ViewT>, "only dimensions 1..4 are supported");
}
}

/// a sampler is a wrapper around an image view that allows to sample from the given image
/// it is additionally templated on an interpolation functor
/// (this enables high-performance implementation of various sampling strategies)
/// NOTE: sampler is non-owning and must thus not outlive the storage behind the image view
///
/// Coordinates:
///
///   pixels are to be considered color samples at integer coordinates
///   thus, linear interpolation of {10.3, 5.8} would consider pixels (10, 5), (10, 6), (11, 5), (11, 6)
///                                             and go 30% x, 80% y
///
/// The interpolator has the signature (pos_t) -> pixel_t
/// NOTE: position is float-based, element is a value
///
/// TODO: allow int based positions, optionally with pixel_t&
/// TODO: derive from accessor for multi arg sample/op()
///
/// Usage:
///
///    auto img = tp::image2_view::...;
///    auto sampler = tp::linear_clamped_px_sampler(img);
///    auto p = sampler({10.3f, 16.7f});
///
/// Naming conventions:
///
///    Filtering:
///      - _linear_ means bi/trilinear filtering
///      - _nearest_ means nearest neighbor filtering
///
///    Lookup:
///      - _clamped_ means coordinates are clamped to edge
///      - _bordered_ means outside of view a user-defined border value is used
///      - _repeated_ means coordinates are repeated
///      - _mirrored_ means coordinates are mirrored once on each side
///      - _mirrored_repeated_ means coordinates are mirrored and repeated
///      - _wrapped_ means coordinates are wrapping around
///
///    Addressing:
///      - _px_ means coordinates are in absolute (potentially fractional) pixels
///      - _uv_ means coordinates are in relative [0..1] coordinates
template <class ViewT, class InterpolatorF>
struct sampler
{
    using view_t = ViewT;
    using pos_t = typename view_t::pos_t;

    explicit sampler(ViewT const& /* used for deduction */, InterpolatorF interpolator) : _interpolator(cc::forward<InterpolatorF>(interpolator)) {}

    auto operator()(pos_t const& p) const { return this->sample(p); }
    auto sample(pos_t const& p) const { return this->_interpolator(p); }

private:
    InterpolatorF _interpolator;
};

template <class PixelT = void, class ViewT>
auto linear_clamped_px_sampler(ViewT view)
{
    using pixel_t = tg::same_or<PixelT, typename ViewT::pixel_t>;
    return sampler(view, interpolation::linear<pixel_t, ViewT>(lookup::clamped<pixel_t>(view)));
}
}
