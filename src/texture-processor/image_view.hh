#pragma once

#include <clean-core/assert.hh>
#include <clean-core/utility.hh>

#include <texture-processor/convert.hh>
#include <texture-processor/detail/accessor.hh>
#include <texture-processor/detail/iterator.hh>
#include <texture-processor/detail/predicates.hh>
#include <texture-processor/extents.hh>
#include <texture-processor/image_metadata.hh>
#include <texture-processor/storage_view.hh>
#include <texture-processor/traits.hh>

// prevent inclusion of more tg
#include <typed-geometry/detail/operators/ops_pos.hh>
#include <typed-geometry/detail/operators/ops_vec.hh>

namespace tp
{
/**
 * a non-owning view onto image data
 * mutable or readonly is communicated via traits (NOT via image_view<T> const)
 */
template <class BaseTraits>
struct image_view : detail::accessor<BaseTraits::dimensions, image_view<BaseTraits>>,           // e.g. img(x, y), img.at(x, y)
                    detail::shape_access<typename BaseTraits::extent_t, image_view<BaseTraits>> // ex img.width(), img.height()
{
    using traits = tp::traits<BaseTraits>;
    using pixel_t = typename traits::pixel_t;
    using pixel_access_t = typename traits::pixel_access_t;
    using pos_t = typename traits::pos_t;
    using ipos_t = typename traits::ipos_t;
    using ivec_t = typename traits::ivec_t;
    using extent_t = typename traits::extent_t;
    using storage_view_t = typename traits::storage_view_t;
    using data_ptr_t = typename storage_view_t::data_ptr_t;
    using accessor_t = detail::accessor<BaseTraits::dimensions, image_view<BaseTraits>>;

    static constexpr int dimensions = traits::dimensions;
    static constexpr int channels = traits::channels;
    static constexpr bool is_mutable = traits::is_writeable;
    static constexpr bool is_readonly = !traits::is_writeable;

    // properties
public:
    /// the size of the image in pixels (per dimension)
    extent_t const& extent() const { return _extent; }

    /// stride used to access storage
    /// TODO: does this apply to z order?
    ivec_t const& byte_stride() const { return _byte_stride; }

    /// returns true if this view has natural stride (contiguous row-by-row)
    /// TODO: also check storage?
    bool has_natural_stride() const { return detail::is_natural_stride(_byte_stride, sizeof(pixel_t), _extent.to_ivec()); }

    /// size in bytes (if this were to be stored compactly)
    size_t byte_size() const { return _extent.pixel_count() * sizeof(pixel_t); }

    /// returns number of pixels
    size_t pixel_count() const { return _extent.pixel_count(); }

    /// returns true if the image view does not contain any pixels
    bool empty() const { return detail::is_any_zero(_extent.to_ivec()); }

    /// returns the raw data pointer where the first pixel of this view lies
    /// NOTE: the actual data might be "behind" this data ptr if the stride is negative
    data_ptr_t data_ptr() const { return _data_ptr; }

    /// creates a metadata entry for this view
    image_metadata metadata() const
    {
        auto md = traits::make_metadata();
        md.extent = tg::ivec4(_extent.to_ivec(), 1);
        md.byte_stride = tg::ivec4(_byte_stride);
        return md;
    }

    // pixel access
public:
    /// returns true iff the given position denotes a valid pixel inside the image
    bool contains(ipos_t const& p) const { return detail::is_non_negative(p) && detail::is_smaller_than(p, _extent.to_ivec()); }

    /// returns true iff the given position denotes a valid pixel on the boundary of the image
    /// TODO: what should this return for array textures? are the layers considered for boundary or not?
    bool is_on_boundary(ipos_t const& p) const { return detail::is_on_boundary(p, _extent.to_ivec()); }

    /// returns the pixel at a given position
    /// NOTE: depending on the storage, this might or might not be a reference
    pixel_access_t at(ipos_t const& p) const
    {
        CC_ASSERT(contains(p));
        return storage_view_t::pixel_at(_data_ptr, p, _byte_stride);
    }
    pixel_access_t at_unchecked(ipos_t const& p) const { return storage_view_t::pixel_at(_data_ptr, p, _byte_stride); }
    pixel_access_t operator[](ipos_t const& p) const { return this->at(p); }
    pixel_access_t operator()(ipos_t const& p) const { return this->at(p); }

    // subviews
    // TODO: how to change between different types?
public:
    /// returns a subview that contains all pixels by start and extent
    [[nodiscard]] image_view subview(ipos_t start, extent_t extent) const
    {
        CC_ASSERT((detail::is_any_zero(extent.to_ivec()) || contains(start)) && "subview out of bounds");
        CC_ASSERT((detail::is_any_zero(extent.to_ivec()) || contains(start + extent.to_ivec() - 1)) && "subview out of bounds");
        image_view v;
        v._data_ptr = _data_ptr + detail::strided_offset(start, _byte_stride);
        v._extent = extent;
        v._byte_stride = _byte_stride;
        return v;
    }
    [[nodiscard]] image_view subview(tg::aabb<dimensions, int> const& bb) const
    {
        return this->subview(bb.min, extent_t::from_ivec(bb.max - bb.min + 1));
    }

    /// returns an image view where the dimension D is mirrored
    /// NOTE: there are also non-templated versions like mirrored_x()
    /// TODO: static assert when storage does not allow this operation
    template <int D>
    [[nodiscard]] image_view mirrored() const
    {
        static_assert(0 <= D && D < dimensions, "invalid dimension");
        auto ev = _extent.to_ivec();
        image_view v = *this; // copy
        if (ev[D] != 0)
        {
            v._data_ptr += v._byte_stride[D] * tg::i64(ev[D] - 1);
            v._byte_stride[D] = -v._byte_stride[D];
        }
        return v;
    }

    /// returns an image view where the dimensions D0 and D1 are swapped
    /// (D0 == D1 is ok and returns this)
    /// NOTE: some image types have restrictions on which dimensions to swap (e.g. #faces in a cubemap)
    /// NOTE: there are also non-templated versions like swapped_xy()
    /// TODO: static assert when storage does not allow this operation
    template <int D0, int D1>
    [[nodiscard]] image_view swapped() const
    {
        static_assert(0 <= D0 && D0 < dimensions, "invalid dimension");
        static_assert(0 <= D1 && D1 < dimensions, "invalid dimension");
        if constexpr (D0 == D1)
            return *this;
        else
        {
            auto ev = _extent.to_ivec();
            cc::swap(ev[D0], ev[D1]);
            image_view v = *this; // copy
            cc::swap(v._byte_stride[D0], v._byte_stride[D1]);
            v._extent = extent_t::from_ivec(ev);
            return v;
        }
    }

    /// returns a sliced view where the given dimension is dropped
    /// (e.g. img.sliced_at<1>(7) of a 2D image returns a view to its 8th row)
    /// NOTE: not all views can be sliced at all dimensions
    /// NOTE: there are also non-templated versions like row(i) or layer(i)
    /// TODO: static assert when storage does not allow this operation
    template <int D>
    [[nodiscard]] image_view<typename traits::template sliced_traits<D>> sliced_at(int i) const
    {
        static_assert(0 <= D && D < dimensions, "invalid dimension");
        static_assert(traits::template can_be_sliced_at<D>);
        CC_ASSERT(0 <= i && i < _extent.to_ivec()[D] && "slice out of bounds");
        using new_view_t = image_view<typename traits::template sliced_traits<D>>;
        new_view_t v;
        v._data_ptr = _data_ptr + _byte_stride[D] * i;
        v._byte_stride = detail::skip_index<D>::value(_byte_stride);
        v._extent = new_view_t::extent_t::from_ivec(detail::skip_index<D>::value(_extent.to_ivec()));
        return v;
    }

    // iteration
public:
    /// returns an iterable that enumerates all pixels and their positions at the same
    /// usage:
    ///    for (auto&& [pos, value] : my_image) { ... }
    auto begin() const -> typename traits::entry_iterator_t { return {_data_ptr, _byte_stride, _extent.to_ivec()}; }
    cc::sentinel end() const { return {}; }

    /// returns an iterable range that enumerates all ipos_t pixel positions in an memory optimized order
    /// usage:
    ///    for (tg::ipos2 p : my_image2.positions()) { ... }
    auto positions() const -> detail::srange<typename traits::position_iterator_t> { return {{_byte_stride, _extent.to_ivec()}}; }

    /// returns an iterable range that visits all pixel values in an memory optimized order
    /// usage:
    ///    for (auto& v : my_image2.pixels())
    ///        v = tg::color3::red;
    auto pixels() const -> detail::srange<typename traits::pixel_iterator_t> { return {{_data_ptr, _byte_stride, _extent.to_ivec()}}; }

    /// iterates over the image (in an optimized order)
    /// calls F for each pixel position
    /// signature: (ipos_t) -> void OR
    ///            (ipos_t, pixel_access_t) -> void
    template <class F>
    void for_each(F&& f)
    {
        for (auto p : this->positions())
            if constexpr (std::is_invocable_v<F, ipos_t>)
                f(p);
            else if constexpr (std::is_invocable_v<F, ipos_t, pixel_access_t>)
                f(p, at_unchecked(p));
            else
                static_assert(cc::always_false<F>, "function must be callable with ipos_t or (ipos_t, pixel_access_t)");
    }

    // modification
public:
    /// overwrites all pixels with the given value
    void fill(pixel_t const& value)
    {
        static_assert(is_mutable, "cannot write to this image");
        for (auto& p : pixels())
            p = value;
    }

    // copying
public:
    /// copies all pixels to the parameter image
    /// NOTE: target must have same dimension and extent
    ///       converter can be used to customize behavior when changing pixel types
    ///       signature: (src_pixel const&) -> target_pixel OR
    ///                  (target_pixel&, src_pixel cont&) -> void
    /// TODO: performance speedup via detecting cases where memcpy works
    ///       performance speedup via block copy when strides mismatch
    template <class RhsTraits, class ConverterT = default_converter>
    void copy_to(image_view<RhsTraits> rhs, ConverterT&& convert = {}) const
    {
        static_assert(rhs.is_mutable, "cannot copy into immutable view");
        static_assert(dimensions == rhs.dimensions, "dimensions must match for copy");
        CC_ASSERT(_extent.to_ivec() == rhs.extent().to_ivec() && "extents must match for copy"); // TODO: log an error with extents WITHOUT including string or format

        for (auto p : this->positions())
        {
            if constexpr (std::is_invocable_r_v<typename image_view<RhsTraits>::pixel_t, ConverterT, pixel_t const&>)
                rhs.at_unchecked(p) = convert(at_unchecked(p));
            else if constexpr (std::is_invocable_v<ConverterT, typename image_view<RhsTraits>::pixel_access_t, pixel_t const&>)
                convert(rhs.at_unchecked(p), at_unchecked(p));
            else
                static_assert(cc::always_false<RhsTraits, ConverterT>, "no suitable call syntax found for converter");
        }
    }
    /// same as copy_to but with reversed roles
    template <class RhsTraits, class ConverterT = default_converter>
    void copy_from(image_view<RhsTraits> rhs, ConverterT&& convert = {}) const
    {
        rhs.copy_to(*this, convert);
    }

    // creation
public:
    /// creates an image view from unchecked raw data
    [[nodiscard]] static image_view from_data(data_ptr_t data, extent_t extent, ivec_t byte_stride)
    {
        image_view v;
        v._data_ptr = data;
        v._extent = extent;
        v._byte_stride = byte_stride;
        return v;
    }

    // unrolled accessors
public:
    using accessor_t::at;
    using accessor_t::operator();

    // members (protected because image derives from this)
protected:
    data_ptr_t _data_ptr = nullptr;
    extent_t _extent;
    ivec_t _byte_stride; // NOTE: in bytes
};
}
