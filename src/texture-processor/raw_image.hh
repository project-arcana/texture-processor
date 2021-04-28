#pragma once

#include <clean-core/array.hh>

#include <texture-processor/image.hh>

namespace tp
{
/**
 * A non-template image that is a thin wrapper around a byte array and metadata
 * This might contain compressed images or user-defined data
 * Can be queried and converted to other images or image_views
 */
struct raw_image
{
public:
    raw_image() = default;
    template<class Traits>
    raw_image(image_view<Traits> img);

private:
    cc::array<std::byte> _data;
    // TODO: what type is the metadata?
};
}
