#pragma once

#include <typed-geometry/tg-lean.hh>

namespace tp
{
/// functor that implements default conversion between pixel types
struct default_converter
{
    template <class TargetT, class SourceT>
    void operator()(TargetT& t, SourceT const& s) const
    {
        static_assert(std::is_constructible_v<TargetT, SourceT const&>, "cannot convert src to target");
        t = TargetT(s);
        // TODO
    }
};
}
