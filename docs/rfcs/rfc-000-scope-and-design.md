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
* block compression formats
* different data layouts (linear, strided, space filling)
* complex pixel types (k-buffer, a-buffer, non-trivial types)
* mapped / projected / procedural textures (aka custom pixel getter / setter)
* sampling (including mipmaps and anisotropic filtering)
* feature-rich image_view type
* layout optimized iteration
* constant-time rotate / mirror / slice / subview (where possible)
* each dimension can be optionally compile-time constant (for improved performance and less image view state)
* the number of channels can be a runtime value
* support broadcasting (i.e. zero strides)
* component-wise operations on images
* creating views to parts of pixels (e.g. a view to the green component of an rgb image)
* masks and selections (and thus restricting algorithms to them)

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

## Example Use Cases

Note that Arcana and thus by extension `tp` is not only targeting games, but also real-time graphics und other interactive applications, like level editors, CAD tools, content processors, etc.
That being said, an unsorted abridged catalog of exemplary use cases:

* Resizing and resampling: An input image should be resized into a different size (upscale, downscale, cropped) and proper resampling is desired (including mipmaps and cubic filtering).
* Heightmap creation: Using raycasting or procedural generation, heightmaps (of terrains or textures) can be created and stored in images.
* Heightmap queries: On heightmaps, many derivative operations apply: tracing a ray in a heightmap, querying a certain height, creating a hierarchical z buffer from a heightmap, creating (relaxed) cones for cone step mapping, computing normal maps from the heightmap, computing ambient occlusion maps.
* Baking: Given a mesh with multiple sets of UV coordinates, decals, painted on colors, ambient occlusion, or other metadata, we want to create a new set of UV coordinates and a set of output textures that have all the previous information "baked in". Other examples include the baking of terrain splat maps, the baking of lightmaps or lightprobes, or of ambient occlusion maps for levels.
* Impostor baking: as part of LOD hierarchies, the highest levels might consist of baked billboards with various attributes. Billboards themselves might be composited / packed into an atlas or an array texture.
* Basic image processing: histogram equalization, contrast/brightness adjustments, morphological operations, "hole-filling" (via push-pull), precomputed alpha, dithering.
* Selections and masking: compositing, modifying, computing with masked subsets of images. E.g. user selections or tasks like "increase contrast of all pixels satisfying condition xyz".
* Texture painting: "drawing" into an image, via 2D or 3D brushes.
* Rasterized plots: some plots such as heatmaps or scatterplots of millions of points are best implemented via an underlying image.
* Compositing and copying parts: When multiple small images should be composed to a larger one (via copy or blend), or parts of one image should be copied into a new image, `image_view`-based pixelwise copying and blending is extremely convenient. Actual use cases include dictionary or atlas texture generation.
* Mipmaps, clipmaps, stacks: different level-of-detail considerations lead to different set-of-textures solutions. Mipmaps for space-saving averaging over arbitrary texture regions, clipmaps for focused multi-resolution, stacks (e.g. Gaussian stacks) for complex image statistics with minimal loss of information.
* Look-up tables: Several textures are lookup tables for various functions, precomputed data, colormaps, gradients. Creating, converting, optimizing these LUTs (e.g. downsample as much as possible while maintaining some error tolerance) should be convenient.
* Integral images: Often, summary statistics (sum, mean, variance) over rectangular image regions are needed. Integral images (or summed area tables) are a useful tool that precomputes a new image (O(n)) and can then answer sum/mean/variance queries for rectangular subviews in O(1).
* Flexible 2D array: in many cases, `image` can be used as a convenient 2D/3D array and `image_view` as the `span<T>` equivalent. This includes level heightmaps, pathfinding metadata, other "information layers" that might be saved per-level.

## Open Questions

* Should a GPU version be a goal? As an extra library?
* Should `tp` depend on `babel` or the other way around?
* Should vector graphics be integrated or is a separate library better?
* Should multisampled textures be supported?
* If yes, are the per-pixel samples of a multisampled textures another dimension or a complex pixel format? (leaning towards the latter)
