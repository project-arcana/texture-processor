#pragma once

#include <cstdint>
#include <type_traits>

#include <typed-geometry/tg-lean.hh>

#include <texture-processor/fwd.hh>

// keep includes minimal
#include <typed-geometry/detail/comp_traits.hh>

namespace tp
{
namespace detail
{
// TODO: make customizable
template <class ElementT>
constexpr int channels_of()
{
    if constexpr (std::is_arithmetic_v<ElementT>) // includes bool
        return 1;
    else
        return tg::detail::comp_size<ElementT>::value;
}
}

namespace base_traits
{
template <class ElementT>
struct linear2D
{
    static_assert(!std::is_reference_v<ElementT>, "cannot store references");

    using element_t = ElementT;
    using extent_t = extent2;
    using storage_t = linear_storage<element_t>;
    using storage_view_t = linear_storage_view<element_t>;
    using element_access_t = element_t&;

    static constexpr int dimensions = 2;
    static constexpr bool is_writeable = !std::is_const_v<element_t>;
    static constexpr bool is_block_based = false;
    static constexpr bool is_strided_linear = true;

    using position_iterator_t = detail::strided_linear_pos_iterator<dimensions>;
    using element_iterator_t = detail::strided_linear_element_iterator<dimensions, storage_view_t>;
};

template <class ElementT>
struct z2D
{
    static_assert(!std::is_reference_v<ElementT>, "cannot store references");

    using element_t = ElementT;
    using extent_t = extent2;
    using storage_t = z_storage<element_t>;
    using storage_view_t = z_storage_view<element_t>;
    using element_access_t = element_t&;

    static constexpr int dimensions = 2;
    static constexpr bool is_writeable = !std::is_const_v<element_t>;
    static constexpr bool is_block_based = false;
    static constexpr bool is_strided_linear = false;

    using position_iterator_t = void; // TODO
};

template <class ElementT, class BlockT>
struct block2D
{
    using element_t = ElementT;
    using block_t = BlockT;
    using extent_t = extent2;
    using storage_t = linear_block_storage<element_t, block_t>;
    using storage_view_t = linear_block_storage_view<element_t, block_t>;
    using element_access_t = element_t;

    static constexpr int dimensions = 2;
    static constexpr bool is_writeable = !std::is_const_v<element_t>;
    static constexpr bool is_block_based = true;
    static constexpr bool is_strided_linear = true;
    static constexpr int block_sizes[] = block_t::block_sizes;

    using position_iterator_t = void; // TODO
};
}

// TODO: "mapping views" (like treating an rgb image as a grayscale image via "map")
// TODO: allow arrays as elements for dynamic channel size
template <class BaseT>
struct traits : BaseT
{
    // NOTE: this should be const if image is not writeable
    using base_t = BaseT;
    using element_t = typename base_t::element_t;

    using ipos_t = tg::pos<base_t::dimensions, int>;
    using ivec_t = tg::vec<base_t::dimensions, int>;
    using pos_t = tg::pos<base_t::dimensions, float>;
    using extent_t = typename base_t::extent_t;
    using storage_t = typename base_t::storage_t;
    using storage_view_t = typename base_t::storage_view_t;

    // TODO: dynamic channels
    static constexpr int channels = detail::channels_of<std::decay_t<element_t>>;

    // sanity checks
    static_assert(BaseT::is_writeable || std::is_const_v<element_t>, "read-only images should have const elements");
};
}
