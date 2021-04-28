#pragma once

#include <cstdint>

namespace tp
{
enum class layout_type : uint8_t
{
    invalid = 0,

    strided_linear = 1,
    z_order = 2,

    custom = 255
};

enum class image_type : uint8_t
{
    invalid = 0,

    image1D,
    image2D,
    image3D,
    imageCube,
};

enum class pixel_format : uint8_t
{
    invalid = 0,

    //
    // raw c++ types
    // (this is only the basic type, the pixel is channels x basic type)
    //
    b1,       // packed bool
    b8,       // bool
    c8,       // char
    i8,       // int8_t
    i16,      // int16_t
    i32,      // int32_t
    i64,      // int64_t
    u8,       // uint8_t
    u16,      // uint16_t
    u32,      // uint32_t
    u64,      // uint64_t
    u8_norm,  // uint8_t (interpreted normalized to 0..1)
    u16_norm, // uint16_t (interpreted normalized to 0..1)
    u32_norm, // uint32_t (interpreted normalized to 0..1)
    u64_norm, // uint64_t (interpreted normalized to 0..1)
    f16,      // tg::half
    f32,      // float
    f64,      // double
    pod,      // a POD type, size given by channels

    //
    // special GPU types
    //

    // sRGB version of regular formats
    rgba8un_srgb,

    // swizzled and irregular formats
    bgra8un,
    b10g11r11uf,

    // block-compressed formats
    bc1_8un,
    bc1_8un_srgb,
    bc2_8un,
    bc2_8un_srgb,
    bc3_8un,
    bc3_8un_srgb,
    bc6h_16f,
    bc6h_16uf,

    // depth formats
    depth32f,
    depth16un,

    // depth stencil formats
    depth32f_stencil8u,
    depth24un_stencil8u,
};

enum class pixel_space : uint8_t
{
    none = 0,

    rgb,
    sRGB,

    hsv,
    hsl,
    yuv,
    cmyk,
    cielab,
    cieluv,
    ciexyz,

    // TODO: more?
    // TODO: alpha?
};

struct image_metadata
{
    image_type type = image_type::invalid;
    layout_type layout = layout_type::invalid;
    tp::pixel_format pixel_format = pixel_format::invalid;
    tp::pixel_space pixel_space = pixel_space::none;

    uint32_t channels = 0;
    uint32_t max_mipmap = 0; // 0 means only LOD 0 is stored

    // support for up to 4 dimensions
    int32_t extent0 = 0;
    int32_t extent1 = 0;
    int32_t extent2 = 0;
    int32_t extent3 = 0;

    // strided linear can have custom strides per dimension
    int32_t stride0 = 0;
    int32_t stride1 = 0;
    int32_t stride2 = 0;
    int32_t stride3 = 0;
};
}
