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
    /// NOTE: currently, only exact matches are allowed, though this will be relaxed in the future
    template <class ImageT>
    bool can_convert_to() const;

    template <class ImageViewT>
    ImageViewT view_as();
    template <class ImageViewT>
    ImageViewT view_as() const;

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

template <class ImageViewT>
bool raw_image::can_view_as() const
{
    static_assert(is_image_view<ImageViewT>, "ImageViewT must be an image_view type");
    using traits = typename ImageViewT::traits;
    image_metadata ref_md = traits::make_metadata();

    if (_metadata.type != ref_md.type)
        return false;
    if (_metadata.layout != ref_md.layout)
        return false;
    if (_metadata.pixel_format != ref_md.pixel_format)
        return false;
    if (_metadata.pixel_space != ref_md.pixel_space)
        return false;
    if (_metadata.byte_per_channel != ref_md.byte_per_channel)
        return false;
    if (_metadata.channels != ref_md.channels)
        return false;
    if (_metadata.max_mipmap != ref_md.max_mipmap)
        return false;

    // extend and strides don't matter here

    return true;
}

template <class ImageT>
bool raw_image::can_convert_to() const
{
    static_assert(is_image<ImageT>, "ImageT must be an image type");
    // using traits = typename ImageT::traits;

    // TODO: for now we need exact matches!
    return can_view_as<typename ImageT::image_view_t>();
}

template <class ImageViewT>
ImageViewT raw_image::view_as()
{
    CC_ASSERT(can_view_as<ImageViewT>());

    using ivec_t = typename ImageViewT::ivec_t;

    auto extent = ImageViewT::extent_t::from_ivec(ivec_t(_metadata.extent));
    auto byte_stride = ivec_t(_metadata.byte_stride);

    return ImageViewT::from_data(_data.data(), extent, byte_stride);
}

template <class ImageViewT>
ImageViewT raw_image::view_as() const
{
    CC_ASSERT(can_view_as<ImageViewT>());
    static_assert(ImageViewT::is_readonly, "raw_image is const, cannot create mutable image_view from it");

    // const_cast is OK here, as we already checked that image view is readonly
    return const_cast<raw_image*>(this)->view_as<ImageViewT>();
}

template <class ImageT>
ImageT raw_image::convert_to() const
{
    CC_ASSERT(can_convert_to<ImageT>());

    // TODO: support actual conversion

    auto view = view_as<typename ImageT::image_view_t>();

    auto img = ImageT::defaulted(view.extent());
    view.copy_to(img);
    return img;
}
}
