#pragma once

#include <cstdint>
#include <type_traits>

#include <typed-geometry/tg-lean.hh>

#include <texture-processor/detail/slicing.hh>
#include <texture-processor/fwd.hh>
#include <texture-processor/image_metadata.hh>
#include <texture-processor/pixel_traits.hh>

namespace tp
{
namespace base_traits
{
template <class PixelT>
struct linear2D
{
    static_assert(!std::is_reference_v<PixelT>, "cannot store references");

    using pixel_t = PixelT;
    using pixel_traits = tp::pixel_traits<std::decay_t<PixelT>>;
    using extent_t = extent2;
    using storage_t = linear_storage<pixel_t>;
    using storage_view_t = linear_storage_view<pixel_t>;
    using pixel_access_t = pixel_t&;

    static constexpr int dimensions = 2;
    static constexpr bool is_writeable = !std::is_const_v<pixel_t>;
    static constexpr bool is_block_based = false;
    static constexpr bool is_strided_linear = true;
    static constexpr tp::image_type image_type = tp::image_type::image2D;
    static constexpr tp::layout_type layout_type = tp::layout_type::strided_linear;

    using position_iterator_t = detail::strided_linear_pos_iterator<dimensions>;
    using pixel_iterator_t = detail::strided_linear_pixel_iterator<dimensions, storage_view_t>;
    using entry_iterator_t = detail::strided_linear_entry_iterator<dimensions, storage_view_t>;
};

template <class PixelT>
struct z2D
{
    static_assert(!std::is_reference_v<PixelT>, "cannot store references");

    using pixel_t = PixelT;
    using pixel_traits = tp::pixel_traits<std::decay_t<PixelT>>;
    using extent_t = extent2;
    using storage_t = z_storage<pixel_t>;
    using storage_view_t = z_storage_view<pixel_t>;
    using pixel_access_t = pixel_t&;

    static constexpr int dimensions = 2;
    static constexpr bool is_writeable = !std::is_const_v<pixel_t>;
    static constexpr bool is_block_based = false;
    static constexpr bool is_strided_linear = false;
    static constexpr tp::image_type image_type = tp::image_type::image2D;
    static constexpr tp::layout_type layout_type = tp::layout_type::z_order;

    using position_iterator_t = void; // TODO
};

template <class PixelT, class BlockT>
struct block2D
{
    using pixel_t = PixelT;
    using pixel_traits = tp::pixel_traits<std::decay_t<PixelT>>;
    using block_t = BlockT;
    using extent_t = extent2;
    using storage_t = linear_block_storage<pixel_t, block_t>;
    using storage_view_t = linear_block_storage_view<pixel_t, block_t>;
    using pixel_access_t = pixel_t;

    static constexpr int dimensions = 2;
    static constexpr bool is_writeable = !std::is_const_v<pixel_t>;
    static constexpr bool is_block_based = true;
    static constexpr bool is_strided_linear = true;
    static constexpr auto block_sizes = block_t::block_sizes;
    static constexpr tp::image_type image_type = tp::image_type::image2D;
    static constexpr tp::layout_type layout_type = tp::layout_type::strided_linear;

    using position_iterator_t = void; // TODO
};
}

// TODO: "mapping views" (like treating an rgb image as a grayscale image via "map")
// TODO: allow arrays as pixels for dynamic channel size
// TODO: optimized power-of-two images?
// TODO: tiled images (each axis independently, maybe only sampler property)
template <class BaseT>
struct traits : BaseT
{
    // NOTE: this should be const if image is not writeable
    using base_t = BaseT;
    using pixel_t = typename base_t::pixel_t;
    using pixel_traits = typename base_t::pixel_traits;

    using ipos_t = tg::pos<base_t::dimensions, int>;
    using ivec_t = tg::vec<base_t::dimensions, int>;
    using pos_t = tg::pos<base_t::dimensions, float>;
    using extent_t = typename base_t::extent_t;
    using storage_t = typename base_t::storage_t;
    using storage_view_t = typename base_t::storage_view_t;

    // TODO: dynamic channels
    static constexpr int channels = pixel_traits::channels;

    template <class NewExtentT>
    using change_extent_t = void; // TODO
    template <class NewPixelT>
    using change_pixel_t = void; // TODO
    template <class NewStorageT>
    using change_storage_t = void; // TODO

    using const_traits = change_pixel_t<pixel_t const>;
    template <int D>
    using sliced_traits = change_extent_t<detail::sliced_extents<extent_t, D>>;
    template <int D>
    static constexpr bool can_be_sliced_at = !std::is_same_v<sliced_traits<D>, void>;

    // sanity checks
    static_assert(BaseT::is_writeable || std::is_const_v<pixel_t>, "read-only images should have const pixels");

    static constexpr image_metadata make_metadata()
    {
        image_metadata md;
        md.channels = channels;
        md.max_mipmap = 0;
        md.byte_per_channel = sizeof(typename pixel_traits::scalar_t);
        md.type = base_t::image_type;
        md.layout = base_t::layout_type;
        md.pixel_space = pixel_traits::space;
        md.pixel_format = pixel_traits::format;
        return md;
    }
};

namespace detail
{
template <class T>
struct is_image_view_t : std::false_type
{
};
template <class Traits>
struct is_image_view_t<image_view<Traits>> : std::true_type
{
};
template <class T>
struct is_image_t : std::false_type
{
};
template <class Traits>
struct is_image_t<image<Traits>> : std::true_type
{
};
}

template <class T>
static constexpr bool is_image = detail::is_image_t<T>::value;
template <class T>
static constexpr bool is_image_view = detail::is_image_view_t<T>::value;
template <class T>
static constexpr bool is_image_or_view = is_image<T> || is_image_view<T>;

template <class ImageOrViewT>
using pixel_type_of = typename ImageOrViewT::pixel_t;
template <class ImageOrViewT>
using image_type_of = image<typename ImageOrViewT::traits>;
template <class ImageOrViewT>
using image_view_type_of = image_view<typename ImageOrViewT::traits>;
}
