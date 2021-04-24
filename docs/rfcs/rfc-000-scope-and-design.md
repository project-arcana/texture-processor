# Scope and Design of Texture Processor

`texture-processor` (aka `tp`) is a library for representation and high-performance typed processing of textures and images on the GPU.

This document outlines the intended scope of `tp` and the high-level design choices derived from the scope.


## Scope

Roughly, `tp` should handle all texture types that can be uploaded onto the GPU.
But it should also be usable as a general-purpose image processing library.

### What should be supported features?

* all GPU pixel formats
* all GPU texture types (1D, 2D, 3D, cubes, arrays)
* mipmapped textures
* multisampled textures
* block compression formats
* different data layouts (linear, strided, space filling)
* complex pixel types (k-buffer, a-buffer, non-trivial types)
* mapped / projected / procedural textures (aka custom pixel getter / setter)
* sampling (including mipmaps and anisotropic filtering)
* feature-rich image_view type
* layout optimized iteration
* constant-time rotate / mirror / slice / subview (where possible)
* each dimension can be optionally compile-time constant (for improved performance and less image view state)
* support broadcasting (i.e. zero strides)
* component-wise operations on images
* creating views to parts of pixels (e.g. a view to the green component of an rgb image)

### What is NOT part of `tp`?

* The actual upload to the GPU is part of the graphics API / library, not `tp`
* As a CPU only library, there are no GPU based image processing algorithms
* IO (and disk formats) is a non-goal as it is part of `babel`
* There are no colors or pixels defined in `tp` as they are taken from `tg`
* Vector graphics are not part of `tp`

## Major Data Structures

In the current design, there are three major data structures in `tp`:

* `raw_image`
* `image`
* `image_view`

### `image_view` (non-owning templated image)

Arguably, the most important one is `image_view`, which is a non-owning view on some raw memory.
It is templated on a trait type that controls many implementation details (like a policy pattern).
This is used to provide a simple, unified interface for image algorithms: they only need a single template argument to generalize over basically all images.

The trait type controls at least:

* what memory layout the image data is stored in?
* what is the pixel type?
* what is the "shape" of the image? (i.e. how many dimensions? how are they called and accessed?)
* how is iteration performed?
* is the data read-write? read-only? can references to the pixels be formed?
* how is the reference to the raw memory stored?

Apart from a raw pointer to the memory, the image view also stores:

* the size of each dimension (width, height, depth, layers, ...)
* mipmap chain parameters if applicable
* getter and setter functions if stateful

`image_view` uses conditional inheritance to make concrete usage as simple as one would expect from a fully specified image class.
This means the user can simply use `img.width`, `img(x, y)`, `img.row(r)` etc.

Non-templated methods that accept `image_view`s typically use the strided memory layout and a fixed pixel type (e.g. a `image2_view<tg::color3>`).
In most cases, this provides good flexibility for minimal performance loss.

### `image` (owning templated image)

An `image` is the owning version of an `image_view` and is implemented as an owning storage (e.g. a vector) and an `image_view` to itself.
Thus, its API is a superset of `image_view`.

It has value semantics, thus copying an `image` copies the content.

### `raw_image` (owning non-templated image)

The templated classes provide the best flexibility and performance, but can be unwieldy when used in type-erased / polymorphic / serialization settings or if the actual memory layout or pixel type is not yet known.
For that, `raw_image` provides a thin layer over a `cc::array<std::byte>`.
Combined with some metadata, a `raw_image` is basically an `image` with all traits only known at runtime.
It is thus trivial to serialize and pass around.

It has a strongly reduced API:

* check if it is compatible with a given trait class (`can_be_viewed_as` and `can_be_converted_to`)
* explicit construction of an `image_view` to this raw image (O(1), asserts if not compatible)
* explicit conversion to an `image` (O(n), asserts if not compatible, but is more lenient than the view as it can actually convert pixels and layouts)
* in-place conversion to a format that supports a given view (i.e. if `can_be_converted_to` but not `can_be_viewed_as`, then this mutates the raw image to something that `can_be_viewed_as` is also true)

`raw_image` can also store otherwise compressed images, such as png or jpg.
This makes viewing basically impossible, but the explicit or in-place conversions still work.
There is also a mechanism for users to dynamically add their own binary formats to `raw_image`.

## Algorithms

To keep compile time low and the library extensible, non-essential methods are implemented as free functions.
The image types only contain the essentials: access, iteration, subview/rotate/mirror/slice, etc.

Otherwise, whenever possible, algorithms should be implemented over an `image_view` with a templated trait type.
From a user perspective, this provides the maximal performance and flexibility.

## Notable Interaction with Other Arcana Libraries

* `tg` is used for sizes, vectors, positions, and colors
* `babel` is used for input / output and disk formats
* `si` uses `tp` for images and icons
* `pv` / `pr` / `phi` accept `tp` to upload textures

## Open Questions

* Should a GPU version be a goal? As an extra library?
* Should `tp` depend on `babel` or the other way around?
* Should vector graphics be integrated or is a separate library better?
* Are the per-pixel samples of a multisampled textures another dimension or a complex pixel format? (leaning towards the latter)
