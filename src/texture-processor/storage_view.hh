#pragma once

#include <typed-geometry/tg-lean.hh>

#include <texture-processor/detail/predicates.hh>

// storage views are trait-like classes (static)
// they provide access into storage given by a data ptr
// NOTE: element_at is called from image_view and is already range-checked
namespace tp
{
template <class T>
struct linear_storage_view
{
    using data_ptr_t = std::conditional_t<std::is_const_v<T>, std::byte const*, std::byte*>;

    template <int D>
    static T& element_at(data_ptr_t data, tg::pos<D, int> p, tg::vec<D, int> stride)
    {
        return *reinterpret_cast<T*>(data + detail::strided_offset(p, stride));
    }
};
template <class T, class BlockT>
struct linear_block_storage_view
{
};
template <class T>
struct z_storage_view
{
};
}
