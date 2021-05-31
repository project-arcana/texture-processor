#pragma once

#include <cstdint>

#include <clean-core/array.hh>

// storage classes manage the backing data of an image
// access into that data is managed by a storage_view
namespace tp
{
template <class T>
struct linear_storage
{
    cc::array<T> data;

    void resize_uninitialized(uint64_t size) { data = cc::array<T>::uninitialized(size); }
    void resize_defaulted(uint64_t size) { data = cc::array<T>::defaulted(size); }
    void resize_filled(uint64_t size, T const& value) { data = cc::array<T>::filled(size, value); }
};

template <class T, class BlockT>
struct linear_block_storage
{
    cc::array<BlockT> data;

    void resize_uninitialized(uint64_t size) { data = cc::array<T>::uninitialized(size); }
    void resize_defaulted(uint64_t size) { data = cc::array<T>::defaulted(size); }
    void resize_filled(uint64_t, T const&) { static_assert(cc::always_false<T, BlockT>, "TODO: implement me"); }
};

template <class T>
struct z_storage
{
    cc::array<T> data;

    void resize_uninitialized(uint64_t size) { data = cc::array<T>::uninitialized(size); }
    void resize_defaulted(uint64_t size) { data = cc::array<T>::defaulted(size); }
    void resize_filled(uint64_t size, T const& value) { data = cc::array<T>::filled(size, value); }
};
}
