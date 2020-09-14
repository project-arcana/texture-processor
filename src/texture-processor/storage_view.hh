#pragma once

#include <typed-geometry/tg-lean.hh>

// storage views are trait-like classes (static)
// they provide access into storage given by a data ptr
// NOTE: element_at is called from image_view and is already range-checked
namespace tp
{
template <class T>
struct linear_storage_view
{
    using data_ptr_t = std::conditional_t<std::is_const_v<T>, std::byte const*, std::byte*>;

    static T& element_at(data_ptr_t data, tg::pos<1, int> p, tg::vec<1, int> stride)
    {
        auto offset = p.x * stride.x;
        return *reinterpret_cast<T*>(data + offset);
    }
    static T& element_at(data_ptr_t data, tg::pos<2, int> p, tg::vec<2, int> stride)
    {
        auto offset = p.x * stride.x + p.y * stride.y;
        return *reinterpret_cast<T*>(data + offset);
    }
    static T& element_at(data_ptr_t data, tg::pos<3, int> p, tg::vec<3, int> stride)
    {
        auto offset = p.x * stride.x + p.y * stride.y + p.z * stride.z;
        return *reinterpret_cast<T*>(data + offset);
    }
    static T& element_at(data_ptr_t data, tg::pos<4, int> p, tg::vec<4, int> stride)
    {
        auto offset = p.x * stride.x + p.y * stride.y + p.z * stride.z + p.w * stride.w;
        return *reinterpret_cast<T*>(data + offset);
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
