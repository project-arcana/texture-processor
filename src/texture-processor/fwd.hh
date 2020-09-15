#pragma once

namespace tp
{
//
// data
//
template <class BaseTraits>
struct image;
template <class BaseTraits>
struct image_view;

//
// extents
//
struct extent1;
struct extent2;
struct extent3;
struct extent2_array;
struct extent_cube;

//
// storage
//
template <class T>
struct linear_storage;
template <class T, class BlockT>
struct linear_block_storage;
template <class T>
struct z_storage;

//
// storage views
//
template <class T>
struct linear_storage_view;
template <class T, class BlockT>
struct linear_block_storage_view;
template <class T>
struct z_storage_view;

//
// iteration
//
namespace detail
{
template <int D>
struct strided_linear_pos_iterator;
template <int D, class StorageViewT>
struct strided_linear_element_iterator;
}

//
// traits
// TODO: better solution
//
template <class BaseT>
struct traits;
namespace base_traits
{
template <class ElementT>
struct linear2D;
template <class ElementT, class BlockT>
struct block2D;
template <class ElementT>
struct z2D;
}

//
// predefined images
//
template <class ElementT>
using image2 = image<base_traits::linear2D<ElementT>>;

}
