#pragma once

#include <typed-geometry/tg-lean.hh>

#include <texture-processor/extents.hh>

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
    decltype(auto) at(int x) const { return static_cast<this_t const*>(this)->at(tg::ipos1(x)); }

    decltype(auto) operator()(int x) { return static_cast<this_t*>(this)->at(tg::ipos1(x)); }
    decltype(auto) operator()(int x) const { return static_cast<this_t const*>(this)->at(tg::ipos1(x)); }

    auto mirrored_x() const { return static_cast<this_t const*>(this)->template mirrored<0>(); }
};

template <class this_t>
struct accessor<2, this_t>
{
    decltype(auto) at(int x, int y) { return static_cast<this_t*>(this)->at(tg::ipos2(x, y)); }
    decltype(auto) at(int x, int y) const { return static_cast<this_t const*>(this)->at(tg::ipos2(x, y)); }

    decltype(auto) operator()(int x, int y) { return static_cast<this_t*>(this)->at(tg::ipos2(x, y)); }
    decltype(auto) operator()(int x, int y) const { return static_cast<this_t const*>(this)->at(tg::ipos2(x, y)); }

    [[nodiscard]] auto mirrored_x() const { return static_cast<this_t const*>(this)->template mirrored<0>(); }
    [[nodiscard]] auto mirrored_y() const { return static_cast<this_t const*>(this)->template mirrored<1>(); }

    [[nodiscard]] auto swapped_xy() const { return static_cast<this_t const*>(this)->template swapped<0, 1>(); }
};

template <class this_t>
struct accessor<3, this_t>
{
    decltype(auto) at(int x, int y, int z) { return static_cast<this_t*>(this)->at(tg::ipos3(x, y, z)); }
    decltype(auto) at(int x, int y, int z) const { return static_cast<this_t const*>(this)->at(tg::ipos3(x, y, z)); }

    decltype(auto) operator()(int x, int y, int z) { return static_cast<this_t*>(this)->at(tg::ipos3(x, y, z)); }
    decltype(auto) operator()(int x, int y, int z) const { return static_cast<this_t const*>(this)->at(tg::ipos3(x, y, z)); }

    [[nodiscard]] auto mirrored_x() const { return static_cast<this_t const*>(this)->template mirrored<0>(); }
    [[nodiscard]] auto mirrored_y() const { return static_cast<this_t const*>(this)->template mirrored<1>(); }
    [[nodiscard]] auto mirrored_z() const { return static_cast<this_t const*>(this)->template mirrored<2>(); }

    [[nodiscard]] auto swapped_xy() const { return static_cast<this_t const*>(this)->template swapped<0, 1>(); }
    [[nodiscard]] auto swapped_xz() const { return static_cast<this_t const*>(this)->template swapped<0, 2>(); }
    [[nodiscard]] auto swapped_yz() const { return static_cast<this_t const*>(this)->template swapped<1, 2>(); }
};

template <class this_t>
struct accessor<4, this_t>
{
    decltype(auto) at(int x, int y, int z, int w) { return static_cast<this_t*>(this)->at(tg::ipos4(x, y, z, w)); }
    decltype(auto) at(int x, int y, int z, int w) const { return static_cast<this_t const*>(this)->at(tg::ipos4(x, y, z, w)); }

    decltype(auto) operator()(int x, int y, int z, int w) { return static_cast<this_t*>(this)->at(tg::ipos4(x, y, z, w)); }
    decltype(auto) operator()(int x, int y, int z, int w) const { return static_cast<this_t const*>(this)->at(tg::ipos4(x, y, z, w)); }

    [[nodiscard]] auto mirrored_x() const { return static_cast<this_t const*>(this)->template mirrored<0>(); }
    [[nodiscard]] auto mirrored_y() const { return static_cast<this_t const*>(this)->template mirrored<1>(); }
    [[nodiscard]] auto mirrored_z() const { return static_cast<this_t const*>(this)->template mirrored<2>(); }
    [[nodiscard]] auto mirrored_w() const { return static_cast<this_t const*>(this)->template mirrored<3>(); }

    [[nodiscard]] auto swapped_xy() const { return static_cast<this_t const*>(this)->template swapped<0, 1>(); }
    [[nodiscard]] auto swapped_xz() const { return static_cast<this_t const*>(this)->template swapped<0, 2>(); }
    [[nodiscard]] auto swapped_yz() const { return static_cast<this_t const*>(this)->template swapped<1, 2>(); }
    // TODO: more of these? maybe not
};

template <class extent_t, class this_t>
struct shape_access;

template <class this_t>
struct shape_access<extent1, this_t>
{
    int width() const { return static_cast<this_t const*>(this)->extent().width; }
};
template <class this_t>
struct shape_access<extent2, this_t>
{
    int width() const { return static_cast<this_t const*>(this)->extent().width; }
    int height() const { return static_cast<this_t const*>(this)->extent().height; }

    [[nodiscard]] auto row(int r) const { return static_cast<this_t const*>(this)->template sliced_at<1>(r); }
    [[nodiscard]] auto column(int c) const { return static_cast<this_t const*>(this)->template sliced_at<0>(c); }
};
template <class this_t>
struct shape_access<extent3, this_t>
{
    int width() const { return static_cast<this_t const*>(this)->extent().width; }
    int height() const { return static_cast<this_t const*>(this)->extent().height; }
    int depth() const { return static_cast<this_t const*>(this)->extent().depth; }
};
template <class this_t>
struct shape_access<extent1_array, this_t>
{
    int width() const { return static_cast<this_t const*>(this)->extent().width; }
    int layers() const { return static_cast<this_t const*>(this)->extent().layers; }

    [[nodiscard]] auto layer(int l) const { return static_cast<this_t const*>(this)->template sliced_at<1>(l); }
};
template <class this_t>
struct shape_access<extent2_array, this_t>
{
    int width() const { return static_cast<this_t const*>(this)->extent().width; }
    int height() const { return static_cast<this_t const*>(this)->extent().height; }
    int layers() const { return static_cast<this_t const*>(this)->extent().layers; }

    [[nodiscard]] auto rows(int r) const { return static_cast<this_t const*>(this)->template sliced_at<1>(r); }
    [[nodiscard]] auto columns(int c) const { return static_cast<this_t const*>(this)->template sliced_at<0>(c); }
    [[nodiscard]] auto layer(int l) const { return static_cast<this_t const*>(this)->template sliced_at<2>(l); }
    [[nodiscard]] auto row(int r, int l) const { return layer(l).row(r); }
    [[nodiscard]] auto column(int c, int l) const { return layer(l).column(c); }
};
template <class this_t>
struct shape_access<extent_cube, this_t>
{
    int size() const { return static_cast<this_t const*>(this)->extent().size; }

    [[nodiscard]] auto rows(int r) const { return static_cast<this_t const*>(this)->template sliced_at<1>(r); }
    [[nodiscard]] auto columns(int c) const { return static_cast<this_t const*>(this)->template sliced_at<0>(c); }
    [[nodiscard]] auto face(int f) const { return static_cast<this_t const*>(this)->template sliced_at<2>(f); }
    [[nodiscard]] auto row(int r, int f) const { return face(f).row(r); }
    [[nodiscard]] auto column(int c, int f) const { return face(f).column(c); }
};
}
