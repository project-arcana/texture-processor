#pragma once

#include <texture-processor/image.hh>
#include <texture-processor/image_view.hh>

/**
 * This file contains various functions to fill images with procedural content
 *
 * Each method has two versions:
 *  * fill_with_xyz(view, ...)          - fills the given view
 *  * generate_xyz<image>(extents, ...) - generates new image
 */

namespace tp
{
/// creates a checkerboard pattern, filling the image alternativ for even and odd pixels
template <class ImageOrViewT>
void fill_with_checkerboard(ImageOrViewT& img, pixel_type_of<ImageOrViewT> const& val_even, pixel_type_of<ImageOrViewT> const& val_odd)
{
    static_assert(is_image_or_view<ImageOrViewT>);

    for (auto&& [pos, value] : img)
        value = detail::pos_sum(pos) % 2 == 0 ? val_even : val_odd;
}

}
