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
 */
template <class BaseTraits>
struct image : image_view<BaseTraits>
{
    using traits = tp::traits<BaseTraits>;
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
