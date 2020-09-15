#pragma once

#include <clean-core/string_view.hh>

#include <texture-processor/image.hh>
#include <texture-processor/image_view.hh>

// reading and loading from a file
// uses babel::image
// NOTE: only file api currently, for more fine-control use babel directly
namespace tp
{
namespace detail
{
bool write_to_file(cc::string_view filename, cc::string_view ext, cc::span<std::byte const> data, int w, int h, int channels, int bit_depth);
}

/// writes an image to a file using the babel image api
/// returns true on success
template <class BaseTraits>
bool write_to_file(image_view<BaseTraits> img, cc::string_view filename)
{
    int dot_idx = -1;
    for (auto i = int(filename.size()) - 1; i >= 0; --i)
    {
        if (filename[i] == '.')
        {
            dot_idx = i;
            break;
        }
        if (filename[i] == '/' || filename[i] == '\\')
            break;
    }
    CC_ASSERT(dot_idx != -1 && "must specify a file extension");
    auto ext = filename.subview(dot_idx + 1);
    auto is_hdr = ext.equals_ignore_case("hdr");
    auto channels = img.channels;

    auto target_element_size = channels * (is_hdr ? 4 : 1);

    CC_ASSERT(img.dimensions == 2 && "only 2D images supported currently");
    auto extents = img.extent().to_ivec();

    // properly strided linear memory can be saved directly
    if (target_element_size == sizeof(typename image_view<BaseTraits>::element_t) && image_view<BaseTraits>::storage_t::is_strided_linear && img.has_natural_stride())
    {
        return detail::write_to_file(filename, ext, cc::span<std::byte const>(img.data_ptr(), img.byte_size()), extents.x, extents.y, channels);
    }
    else // otherwise we need to copy and covert
    {
        if (is_hdr)
        {
            using tmp_traits = base_traits::linear2D<tg::comp<image_view<BaseTraits>::channels, float>>;
            auto tmp = image<tmp_traits>::uninitialized(img.extent());
            CC_ASSERT(tmp.has_natural_stride());
            img.copy_to(tmp);
            return detail::write_to_file(filename, ext, cc::span<std::byte const>(tmp.data_ptr(), tmp.byte_size()), extents.x, extents.y, channels);
        }
        else
        {
            using tmp_traits = base_traits::linear2D<tg::comp<image_view<BaseTraits>::channels, uint8_t>>;
            auto tmp = image<tmp_traits>::uninitialized(img.extent());
            CC_ASSERT(tmp.has_natural_stride());
            img.copy_to(tmp);
            return detail::write_to_file(filename, ext, cc::span<std::byte const>(tmp.data_ptr(), tmp.byte_size()), extents.x, extents.y, channels);
        }
    }
}
}
