#include "raw_image.hh"

tp::raw_image::raw_image(tp::image_metadata metadata, cc::array<std::byte> data) : _metadata(cc::move(metadata)), _data(cc::move(data)) {}

tp::raw_image::raw_image(tp::image_metadata metadata, cc::span<const std::byte> data) : _metadata(cc::move(metadata))
{
    _data = cc::array<std::byte>::uninitialized(data.size());
    data.copy_to(_data);
}
