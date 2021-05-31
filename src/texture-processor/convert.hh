#pragma once

#include <typed-geometry/tg-lean.hh>

namespace tp
{
namespace detail
{
template <class T>
constexpr bool is_normalized_u8()
{
    if constexpr (std::is_same_v<T, tg::u8>)
        return true;
    else if constexpr (tg::detail::comp_size<T>::value > 0)
        return is_normalized_u8<std::decay_t<decltype(std::declval<T>()[0])>>();
    else
        return false;
}
template <class T>
constexpr bool is_normalized_u16()
{
    if constexpr (std::is_same_v<T, tg::u16>)
        return true;
    else if constexpr (tg::detail::comp_size<T>::value > 0)
        return is_normalized_u16<std::decay_t<decltype(std::declval<T>()[0])>>();
    else
        return false;
}
template <class T>
constexpr bool is_normalized_i8()
{
    if constexpr (std::is_same_v<T, tg::i8>)
        return true;
    else if constexpr (tg::detail::comp_size<T>::value > 0)
        return is_normalized_i8<std::decay_t<decltype(std::declval<T>()[0])>>();
    else
        return false;
}
template <class T>
constexpr bool is_normalized_i16()
{
    if constexpr (std::is_same_v<T, tg::i16>)
        return true;
    else if constexpr (tg::detail::comp_size<T>::value > 0)
        return is_normalized_i16<std::decay_t<decltype(std::declval<T>()[0])>>();
    else
        return false;
}
template <class T>
constexpr bool is_floating_point()
{
    if constexpr (std::is_same_v<T, tg::f32>)
        return true;
    else if constexpr (std::is_same_v<T, tg::f64>)
        return true;
    else if constexpr (tg::detail::comp_size<T>::value > 0)
        return is_floating_point<std::decay_t<decltype(std::declval<T>()[0])>>();
    else
        return false;
}
}

/// functor that implements default conversion between pixel types
/// NOTE: per default, u8 and u16 are seen as [0..1] floats
/// NOTE: currently needs at least tg/feature/vector for some vector valued conversions
struct default_converter
{
    template <class TargetT, class SourceT>
    void operator()(TargetT& t, SourceT const& s) const
    {
        if constexpr (detail::is_floating_point<TargetT>() && detail::is_normalized_u8<SourceT>())
        {
            t = TargetT(s) / tg::max<tg::u8>();
        }
        else if constexpr (detail::is_floating_point<TargetT>() && detail::is_normalized_u16<SourceT>())
        {
            t = TargetT(s) / tg::max<tg::u16>();
        }
        else if constexpr (detail::is_normalized_u8<TargetT>() && detail::is_floating_point<SourceT>())
        {
            // TODO: clamp=
            t = TargetT(s * tg::max<tg::u8>());
        }
        else if constexpr (detail::is_normalized_u16<TargetT>() && detail::is_floating_point<SourceT>())
        {
            // TODO: clamp?
            t = TargetT(s * tg::max<tg::u16>());
        }
        // TODO: u8 vs u16 cases?
        // TODO: iN cases?
        else
        {
            static_assert(std::is_constructible_v<TargetT, SourceT const&>, "cannot convert src to target");
            t = TargetT(s);
            // TODO: float -> 8bit or 16bit int should denormalize
        }
    }
};
}
