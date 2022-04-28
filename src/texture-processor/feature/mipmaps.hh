#pragma once

#include <clean-core/optional.hh>
#include <clean-core/vector.hh>

#include <texture-processor/feature/resampling.hh>
#include <texture-processor/image.hh>
#include <texture-processor/image_view.hh>

namespace tp
{
template <class ImageOrViewT>
[[nodiscard]] auto generate_mipmaps_by_averaging(ImageOrViewT const& img, cc::optional<int> max_level = {}) -> cc::vector<image_type_of<ImageOrViewT>>
{
    static_assert(is_image_or_view<ImageOrViewT>);
    using image_t = image_type_of<ImageOrViewT>;

    cc::vector<image_t> res;
    res.push_back(image_t(img));
    while ((!max_level.has_value() || int(res.size()) < max_level.value()) && //
           detail::downsampled_extent(res.back().extent()) != res.back().extent())
        res.push_back(tp::downsample_by_2x_averaging(res.back()));
    return res;
}
}
