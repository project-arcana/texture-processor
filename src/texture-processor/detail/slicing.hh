#pragma once

#include <texture-processor/extents.hh>

namespace tp::detail
{
template <class extent_t, int D>
struct sliced_extents
{
    using type = void; // not supported
};

template <int D>
struct sliced_extents<extent2, D>
{
    using type = extent1;
};

template <int D>
struct sliced_extents<extent3, D>
{
    using type = extent2;
};

template <>
struct sliced_extents<extent2_array, 0>
{
    using type = extent1_array;
};
template <>
struct sliced_extents<extent2_array, 1>
{
    using type = extent1_array;
};
template <>
struct sliced_extents<extent2_array, 2>
{
    using type = extent2;
};

template <>
struct sliced_extents<extent_cube, 0>
{
    using type = extent1_array;
};
template <>
struct sliced_extents<extent_cube, 1>
{
    using type = extent1_array;
};
template <>
struct sliced_extents<extent_cube, 2>
{
    using type = extent2;
};
}
