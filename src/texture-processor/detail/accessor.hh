#pragma once

#include <typed-geometry/tg-lean.hh>

// TODO: variable names could be even better
// TODO: add other functions like contains
namespace tp::detail
{
template <int D, class this_t>
struct accessor;

template <class this_t>
struct accessor<1, this_t>
{
    decltype(auto) at(int x) { return static_cast<this_t*>(this)->at(tg::ipos1(x)); }
    decltype(auto) at(int x) const { return static_cast<this_t*>(this)->at(tg::ipos1(x)); }

    decltype(auto) operator()(int x) { return static_cast<this_t*>(this)->at(tg::ipos1(x)); }
    decltype(auto) operator()(int x) const { return static_cast<this_t*>(this)->at(tg::ipos1(x)); }
};

template <class this_t>
struct accessor<2, this_t>
{
    decltype(auto) at(int x, int y) { return static_cast<this_t*>(this)->at(tg::ipos2(x, y)); }
    decltype(auto) at(int x, int y) const { return static_cast<this_t*>(this)->at(tg::ipos2(x, y)); }

    decltype(auto) operator()(int x, int y) { return static_cast<this_t*>(this)->at(tg::ipos2(x, y)); }
    decltype(auto) operator()(int x, int y) const { return static_cast<this_t*>(this)->at(tg::ipos2(x, y)); }
};

template <class this_t>
struct accessor<3, this_t>
{
    decltype(auto) at(int x, int y, int z) { return static_cast<this_t*>(this)->at(tg::ipos3(x, y, z)); }
    decltype(auto) at(int x, int y, int z) const { return static_cast<this_t*>(this)->at(tg::ipos3(x, y, z)); }

    decltype(auto) operator()(int x, int y, int z) { return static_cast<this_t*>(this)->at(tg::ipos3(x, y, z)); }
    decltype(auto) operator()(int x, int y, int z) const { return static_cast<this_t*>(this)->at(tg::ipos3(x, y, z)); }
};

template <class this_t>
struct accessor<4, this_t>
{
    decltype(auto) at(int x, int y, int z, int w) { return static_cast<this_t*>(this)->at(tg::ipos4(x, y, z, w)); }
    decltype(auto) at(int x, int y, int z, int w) const { return static_cast<this_t*>(this)->at(tg::ipos4(x, y, z, w)); }

    decltype(auto) operator()(int x, int y, int z, int w) { return static_cast<this_t*>(this)->at(tg::ipos4(x, y, z, w)); }
    decltype(auto) operator()(int x, int y, int z, int w) const { return static_cast<this_t*>(this)->at(tg::ipos4(x, y, z, w)); }
};
}
