#pragma once

#include <cstdint>

#include <typed-geometry/tg-lean.hh>

// TODO: implicit conversions from tg::size types where appropriate
// TODO: convert to proper xD (e.g. extent_cube to ivec3)
namespace tp
{
struct extent1
{
    int width = 0;

    tg::ivec1 to_ivec() const { return {width}; }
    uint64_t elements() const { return width; }
};

struct extent2
{
    int width = 0;
    int height = 0;

    tg::ivec2 to_ivec() const { return {width, height}; }
    uint64_t elements() const { return width * height; }
};

struct extent3
{
    int width = 0;
    int height = 0;
    int depth = 0;

    tg::ivec3 to_ivec() const { return {width, height, depth}; }
    uint64_t elements() const { return width * height * depth; }
};

struct extent2_array
{
    int width = 0;
    int height = 0;
    int layers = 0;

    tg::ivec3 to_ivec() const { return {width, height, layers}; }
    uint64_t elements() const { return width * height * layers; }
};

struct extent_cube
{
    int size = 0; // size x size square

    tg::ivec3 to_ivec() const { return {size, size, 6}; }
    uint64_t elements() const { return size * size * 6; }
};
}
