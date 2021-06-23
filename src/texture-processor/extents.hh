#pragma once

#include <cstdint>

#include <clean-core/assert.hh>

#include <typed-geometry/tg-lean.hh>

// TODO: implicit conversions from tg::size types where appropriate
// TODO: convert to proper xD (e.g. extent_cube to ivec3)
// NOTE: when adding a new extent, don't forge to add it to shape_access in detail/accessor
namespace tp
{
struct extent1
{
    int width = 0;

    tg::ivec1 to_ivec() const { return {width}; }
    static extent1 from_ivec(tg::ivec1 const& e) { return {e.x}; }
    uint64_t pixel_count() const { return width; }

    constexpr bool operator==(extent1 const& rhs) const { return width == rhs.width; }
    constexpr bool operator!=(extent1 const& rhs) const { return width != rhs.width; }
};

struct extent2
{
    int width = 0;
    int height = 0;

    tg::ivec2 to_ivec() const { return {width, height}; }
    static extent2 from_ivec(tg::ivec2 const& e) { return {e.x, e.y}; }
    uint64_t pixel_count() const { return width * height; }

    constexpr bool operator==(extent2 const& rhs) const { return width == rhs.width && height == rhs.height; }
    constexpr bool operator!=(extent2 const& rhs) const { return width != rhs.width || height != rhs.height; }
};

struct extent3
{
    int width = 0;
    int height = 0;
    int depth = 0;

    tg::ivec3 to_ivec() const { return {width, height, depth}; }
    static extent3 from_ivec(tg::ivec3 const& e) { return {e.x, e.y, e.z}; }
    uint64_t pixel_count() const { return width * height * depth; }

    constexpr bool operator==(extent3 const& rhs) const { return width == rhs.width && height == rhs.height && depth == rhs.depth; }
    constexpr bool operator!=(extent3 const& rhs) const { return width != rhs.width || height != rhs.height || depth != rhs.depth; }
};

struct extent1_array
{
    int width = 0;
    int layers = 0;

    tg::ivec2 to_ivec() const { return {width, layers}; }
    static extent1_array from_ivec(tg::ivec2 const& e) { return {e.x, e.y}; }
    uint64_t pixel_count() const { return width * layers; }

    constexpr bool operator==(extent1_array const& rhs) const { return width == rhs.width && layers == rhs.layers; }
    constexpr bool operator!=(extent1_array const& rhs) const { return width != rhs.width || layers != rhs.layers; }
};

struct extent2_array
{
    int width = 0;
    int height = 0;
    int layers = 0;

    tg::ivec3 to_ivec() const { return {width, height, layers}; }
    static extent2_array from_ivec(tg::ivec3 const& e) { return {e.x, e.y, e.z}; }
    uint64_t pixel_count() const { return width * height * layers; }

    constexpr bool operator==(extent2_array const& rhs) const { return width == rhs.width && height == rhs.height && layers == rhs.layers; }
    constexpr bool operator!=(extent2_array const& rhs) const { return width != rhs.width || height != rhs.height || layers != rhs.layers; }
};

struct extent_cube
{
    int size = 0; // size x size square

    tg::ivec3 to_ivec() const { return {size, size, 6}; }
    static extent_cube from_ivec(tg::ivec3 const& e)
    {
        CC_ASSERT(e.x == e.y && "cubemaps are always square");
        CC_ASSERT(e.z == 6 && "cubemaps have 6 faces");
        return {e.x};
    }
    uint64_t pixel_count() const { return size * size * 6; }

    constexpr bool operator==(extent_cube const& rhs) const { return size == rhs.size; }
    constexpr bool operator!=(extent_cube const& rhs) const { return size != rhs.size; }
};
}
