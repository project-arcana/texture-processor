#pragma once

#include <clean-core/array.hh>
#include <clean-core/span.hh>

#include <texture-processor/image.hh>
#include <texture-processor/image_metadata.hh>

namespace tp
{
/**
 * A non-template image that is a thin wrapper around a byte array and metadata
 * This might contain compressed images or user-defined data
 * Can be queried and converted to other images or image_views
 *
 * NOTE: const-ness is respected, raw_image const& is read-only
 *
 * TODO: a copy_to?
 */
struct raw_image
{
    // ctors
public:
    raw_image() = default;
    raw_image(raw_image&&) = default;
    raw_image(raw_image const&) = default;
    raw_image& operator=(raw_image&&) = default;
    raw_image& operator=(raw_image const&) = default;

    raw_image(image_metadata metadata, cc::array<std::byte> data);
    raw_image(image_metadata metadata, cc::span<std::byte const> data);

    /// converts an image_view to a raw image
    /// NOTE: this performs a copy of the image data
    template <class Traits>
    raw_image(image_view<Traits> img);

    // queries
public:
    bool empty() const { return _data.empty(); }
    size_t size_bytes() const { return _data.size(); }

    image_metadata const& metadata() const { return _metadata; }

    /// returns a view into the image data, i.e. the raw_image must outlive the result!
    cc::span<std::byte> raw_data() { return _data; }
    cc::span<std::byte const> raw_data() const { return _data; }

    // conversions
public:
    /// returns true, if .view_as<ImageViewT>() will succeed
    /// NOTE: you still need to respect proper const-ness
    template <class ImageViewT>
    bool can_view_as() const;

    /// returns true, if .convert_to<ImageT>() will succeed
    /// NOTE: you still need to respect proper const-ness
    template <class ImageT>
    bool can_convert_to() const;

    template <class ImageViewT>
    ImageViewT view_as();
    template <class ImageViewT>
    ImageViewT view_as() const;

    template <class ImageT>
    ImageT convert_to();
    template <class ImageT>
    ImageT convert_to() const;

private:
    image_metadata _metadata;
    cc::array<std::byte> _data;
};

template <class Traits>
raw_image::raw_image(image_view<Traits> img)
{
    static_assert(std::is_trivially_copyable_v<typename Traits::pixel_t>, "only works for trivially copyable types currently");

    // init metadata
    _metadata = img.metadata();
    _metadata.byte_stride = detail::natural_stride_for(_metadata.byte_per_channel * _metadata.channels, _metadata.extent);

    // copy data
    _data = cc::array<std::byte>::uninitialized(img.byte_size());
    auto target = image_view<Traits>::from_data(_data.data(), img.extent(), img.byte_stride());
    img.copy_to(target);
}
}
