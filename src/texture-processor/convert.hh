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
        t = s;
        // TODO
    }
};
}
