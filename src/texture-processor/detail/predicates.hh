#pragma once

#include <typed-geometry/tg-lean.hh>

namespace tp::detail
{
constexpr bool is_non_negative(tg::ipos1 const& p) { return p.x >= 0; }
constexpr bool is_non_negative(tg::ipos2 const& p) { return p.x >= 0 && p.y >= 0; }
constexpr bool is_non_negative(tg::ipos3 const& p) { return p.x >= 0 && p.y >= 0 && p.z >= 0; }
constexpr bool is_non_negative(tg::ipos4 const& p) { return p.x >= 0 && p.y >= 0 && p.z >= 0 && p.w >= 0; }

constexpr bool is_smaller_than(tg::ipos1 const& p, tg::ivec1 const& e) { return p.x < e.x; }
constexpr bool is_smaller_than(tg::ipos2 const& p, tg::ivec2 const& e) { return p.x < e.x && p.y < e.y; }
constexpr bool is_smaller_than(tg::ipos3 const& p, tg::ivec3 const& e) { return p.x < e.x && p.y < e.y && p.z < e.z; }
constexpr bool is_smaller_than(tg::ipos4 const& p, tg::ivec4 const& e) { return p.x < e.x && p.y < e.y && p.z < e.z && p.w < e.w; }

constexpr bool is_on_boundary(tg::ipos1 const& p, tg::ivec1 const& e) { return p.x == 0 || p.x == e.x - 1; }
constexpr bool is_on_boundary(tg::ipos2 const& p, tg::ivec2 const& e) { return p.x == 0 || p.y == 0 || p.x == e.x - 1 || p.y == e.y - 1; }
constexpr bool is_on_boundary(tg::ipos3 const& p, tg::ivec3 const& e)
{
    return p.x == 0 || p.y == 0 || p.z == 0 || p.x == e.x - 1 || p.y == e.y - 1 || p.z == e.z - 1;
}
constexpr bool is_on_boundary(tg::ipos4 const& p, tg::ivec4 const& e)
{
    return p.x == 0 || p.y == 0 || p.z == 0 || p.w == 0 || p.x == e.x - 1 || p.y == e.y - 1 || p.z == e.z - 1 || p.w == e.w - 1;
}

constexpr tg::ivec1 natural_stride_for(int s, tg::ivec1 const&) { return {s}; }
constexpr tg::ivec2 natural_stride_for(int s, tg::ivec2 const& e) { return {s, e.x * s}; }
constexpr tg::ivec3 natural_stride_for(int s, tg::ivec3 const& e) { return {s, e.x * s, e.x * e.y * s}; }
constexpr tg::ivec4 natural_stride_for(int s, tg::ivec4 const& e) { return {s, e.x * s, e.x * e.y * s, e.x * e.y * e.z * s}; }
}
