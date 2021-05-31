#pragma once

#include <typed-geometry/tg-lean.hh>

// keep includes minimal
#include <typed-geometry/detail/comp_traits.hh>

#include <texture-processor/image_metadata.hh>

namespace tp
{
/// customizable traits type for pixels
/// NOTE: second template arg can be used for SFINAE
template <class T, class = void>
struct pixel_traits;

namespace detail
{
template <class PixelT>
constexpr int infer_channels_of()
{
    if constexpr (std::is_arithmetic_v<PixelT>) // includes bool
        return 1;
    else
        return tg::detail::comp_size<PixelT>::value;

    // TODO: better error message
}

template <class T>
struct scalar_pixel_format
{
    static_assert(tg::always_false<T>, "type not supported");
};

#define TP_DEFINE_SCALAR_PIXEL_FORMAT(TYPE, ENUM)                 \
    template <>                                                   \
    struct scalar_pixel_format<TYPE>                              \
    {                                                             \
        static constexpr pixel_format value = pixel_format::ENUM; \
    };

TP_DEFINE_SCALAR_PIXEL_FORMAT(bool, b8);
TP_DEFINE_SCALAR_PIXEL_FORMAT(char, c8);
TP_DEFINE_SCALAR_PIXEL_FORMAT(int8_t, i8);
TP_DEFINE_SCALAR_PIXEL_FORMAT(int16_t, i16);
TP_DEFINE_SCALAR_PIXEL_FORMAT(int32_t, i32);
TP_DEFINE_SCALAR_PIXEL_FORMAT(int64_t, i64);
TP_DEFINE_SCALAR_PIXEL_FORMAT(uint8_t, u8);
TP_DEFINE_SCALAR_PIXEL_FORMAT(uint16_t, u16);
TP_DEFINE_SCALAR_PIXEL_FORMAT(uint32_t, u32);
TP_DEFINE_SCALAR_PIXEL_FORMAT(uint64_t, u64);
TP_DEFINE_SCALAR_PIXEL_FORMAT(tg::half, f16);
TP_DEFINE_SCALAR_PIXEL_FORMAT(float, f32);
TP_DEFINE_SCALAR_PIXEL_FORMAT(double, f64);

template <class PixelT>
constexpr tp::pixel_format infer_pixel_format_of()
{
    if constexpr (std::is_arithmetic_v<PixelT>) // includes bool
        return scalar_pixel_format<PixelT>::value;
    else
        return scalar_pixel_format<std::remove_cv_t<decltype(std::declval<PixelT>()[0])>>::value;
}

template <class PixelT, class = void>
struct pixel_scalar_t
{
    using type = std::remove_cv_t<decltype(std::declval<PixelT>()[0])>;
};
template <class T>
struct pixel_scalar_t<T, std::enable_if_t<std::is_arithmetic_v<T>>>
{
    using type = T;
};
}

template <class T>
struct inferred_pixel_traits
{
    static_assert(!std::is_const_v<T>, "T must not be const");
    static_assert(!std::is_reference_v<T>, "T must not be a ref");

    using scalar_t = typename detail::pixel_scalar_t<T>::type;
    static constexpr int channels = detail::infer_channels_of<T>();
    static constexpr pixel_space space = pixel_space::none;
    static constexpr pixel_format format = detail::infer_pixel_format_of<T>();
};

// default impl: defer to inferred_pixel_traits
template <class T, class>
struct pixel_traits : inferred_pixel_traits<T>
{
};

template <int D, class T>
struct pixel_traits<tg::color<D, T>> : inferred_pixel_traits<tg::color<D, T>>
{
    using scalar_t = T;
    static constexpr int channels = D;
    static constexpr pixel_space space = pixel_space::rgb;
    static constexpr pixel_format format = detail::infer_pixel_format_of<T>();
};
}
