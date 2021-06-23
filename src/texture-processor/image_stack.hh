#pragma once

#include <clean-core/span.hh>
#include <clean-core/vector.hh>

#include <texture-processor/image.hh>

namespace tp
{
/// An image stack is an array of images of homogeneous type but potentially varying sizes
/// Famous examples are mip-maps or Gaussian stacks
///
/// TODO: is this needed or are spans of images/views sufficient?
template <class Traits>
struct image_stack
{
    // TODO: const version where image is not readable
    cc::span<image<Traits> const> images() { return _images; }

    void add(image<Traits> img) { _images.push_back(cc::move(img)); }

    void clear() { _images.clear(); }

    size_t size() { return _images.size(); }

private:
    cc::vector<image<Traits>> _images;
};
}
