#pragma once

#include <texture-processor/image_view.hh>
#include <texture-processor/storage.hh>

namespace tp
{
/**
 * An image is an image_view with an associated storage
 * Basically the value-semantics owning version of image_view
 * It can also be thought of as a cc::vector but with an image API
 *
 * TODO:
 *  - how are non-natural strides handled in creation/resize?
 *
 * NOTE: views into this image remain valid if the image is moved!
 */
template <class BaseTraits>
struct image : image_view<BaseTraits>
{
    using traits = tp::traits<BaseTraits>;
    using image_view_t = image_view<BaseTraits>;
    using pixel_t = typename traits::pixel_t;
    using pos_t = typename traits::pos_t;
    using ipos_t = typename traits::ipos_t;
    using extent_t = typename traits::extent_t;
    using storage_t = typename traits::storage_t;
    using storage_view_t = typename traits::storage_view_t;
    using data_ptr_t = typename storage_view_t::data_ptr_t;

    // ctors and creation
public:
    image() = default;
    ~image() = default;

    // moving storage does not change the data_ptr
    image(image&& rhs) noexcept = default;
    image& operator=(image&& rhs) noexcept = default;

    // copy results in natural stride again
    // TODO: would it be better to retain original stride?
    image(image const& rhs)
    {
        // TODO: replace by unitialized and copy_construct_to
        resize(rhs.extent());
        rhs.copy_to(*this);
    }
    image& operator=(image const& rhs)
    {
        resize(rhs.extent());
        rhs.copy_to(*this);
    }

    /// copies the view into this image
    explicit image(image_view<BaseTraits> view)
    {
        resize(view.extent());
        view.copy_to(*this);
    }

    /// creates a new image of the desired size and initializes it with the provided value
    [[nodiscard]] static image filled(extent_t e, pixel_t const& initial_value)
    {
        image img;
        img.resize(e, initial_value);
        return img;
    }
    /// creates a new image of the desired size and calls the default ctor for each pixel
    [[nodiscard]] static image defaulted(extent_t e)
    {
        image img;
        img.resize(e);
        return img;
    }
    /// creates a new image of the desired size, leaving the data uninitialized
    /// CAUTION: this can easily lead to UB, especially with non-trivial types
    [[nodiscard]] static image uninitialized(extent_t e)
    {
        image img;
        img.resize_uninitialized(e);
        return img;
    }

    // TODO: this does not preserve content, which might be weird
    void resize(extent_t e)
    {
        this->_storage.resize_defaulted(e.pixel_count());
        this->init_data_view(e);
    }
    void resize(extent_t e, pixel_t const& fill_value)
    {
        this->_storage.resize_filled(e.pixel_count(), fill_value);
        this->init_data_view(e);
    }
    void resize_uninitialized(extent_t e)
    {
        this->_storage.resize_uninitialized(e.pixel_count());
        this->init_data_view(e);
    }

    // view
public:
    image_view<BaseTraits> view() { return *this; }

    // helper
private:
    void init_data_view(extent_t e)
    {
        // TODO: stride for block and z order storage
        this->_extent = e;
        this->_data_ptr = reinterpret_cast<data_ptr_t>(this->_storage.data.data());
        this->_byte_stride = detail::natural_stride_for(sizeof(pixel_t), e.to_ivec());
    }

    // members
    // NOTE: only storage because rest in saved in the view
private:
    storage_t _storage;
};

}
