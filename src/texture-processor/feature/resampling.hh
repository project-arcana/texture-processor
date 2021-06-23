#pragma once

#include <texture-processor/image.hh>
#include <texture-processor/image_view.hh>

namespace tp
{
namespace detail
{
inline int div2min1(int v) { return v > 1 ? v >> 1 : 1; }
inline extent1 downsampled_extent(extent1 const& e) { return {div2min1(e.width)}; }
inline extent2 downsampled_extent(extent2 const& e) { return {div2min1(e.width), div2min1(e.height)}; }
inline extent3 downsampled_extent(extent3 const& e) { return {div2min1(e.width), div2min1(e.height), div2min1(e.depth)}; }
inline extent1_array downsampled_extent(extent1_array const& e) { return {div2min1(e.width), e.layers}; }
inline extent2_array downsampled_extent(extent2_array const& e) { return {div2min1(e.width), div2min1(e.height), e.layers}; }
inline extent_cube downsampled_extent(extent_cube const& e) { return {div2min1(e.size)}; }

template <class ImageOrViewT>
auto downsample_at()
}

/// creates a downsampled image by averaging 2x2 regions (or 2x2x2 for 3D)
/// if image has odd extents, the regions are smaller
template <class ImageOrViewT>
[[nodiscard]] auto downsample_by_2x_averaging(ImageOrViewT const& img) -> image_type_of<ImageOrViewT>
{
    static_assert(is_image_or_view<ImageOrViewT>);

    using extent_t = typename ImageOrViewT::extent_t;

    // TODO: maybe a generic downsample-dimensions?

    auto res_extent = detail::downsampled_extent(img.extent());
    auto res = image_type_of<ImageOrViewT>::defaulted(res_extent);
    for (auto&& [pos, value] : res) {

    }
    return res;
}
}
