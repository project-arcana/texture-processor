#pragma once

#include <clean-core/assert.hh>

#include <texture-processor/detail/accessor.hh>
#include <texture-processor/detail/predicates.hh>
#include <texture-processor/extents.hh>
#include <texture-processor/storage_view.hh>
#include <texture-processor/traits.hh>

namespace tp
{
/**
 * a non-owning view onto image data
 * mutable or readonly is communicated via traits (NOT via image_view<T> const)
 */
template <class BaseTraits>
struct image_view : detail::accessor<BaseTraits::dimensions, image_view<BaseTraits>>
{
    using traits = traits<BaseTraits>;
    using element_t = typename traits::element_t;
    using element_access_t = typename traits::element_access_t;
    using pos_t = typename traits::pos_t;
    using ipos_t = typename traits::ipos_t;
    using ivec_t = typename traits::ivec_t;
    using extent_t = typename traits::extent_t;
    using storage_view_t = typename traits::storage_view_t;
    using data_ptr_t = typename storage_view_t::data_ptr_t;
    using accessor_t = detail::accessor<BaseTraits::dimensions, image_view<BaseTraits>>;

    static constexpr bool is_mutable = traits::is_writeable;
    static constexpr bool is_readonly = !traits::is_writeable;

    // properties
public:
    /// the size of the image in pixels (per dimension)
    extent_t const& extent() const { return _extent; }
    /// stride used to access storage
    /// TODO: does this apply to z order?
    ivec_t const& stride() const { return _stride; }

    // pixel access
public:
    /// returns true iff the given position denotes a valid pixel inside the image
    bool contains(ipos_t const& p) const { return detail::is_non_negative(p) && detail::is_smaller_than(p, _extent.to_ivec()); }

    /// returns true iff the given position denotes a valid pixel on the boundary of the image
    /// TODO: what should this return for array textures? are the layers considered for boundary or not?
    bool is_on_boundary(ipos_t const& p) const { return detail::is_on_boundary(p); }

    /// returns the element at a given position
    /// NOTE: depending on the storage, this might be a reference
    element_access_t at(ipos_t const& p) const
    {
        CC_ASSERT(contains(p));
        return storage_view_t::element_at(_data_ptr, p, _stride);
    }
    element_access_t operator[](ipos_t const& p) const { return this->at(p); }
    element_access_t operator()(ipos_t const& p) const { return this->at(p); }

    // creation
public:
    /// creates an image view from unchecked raw data
    static image_view from_data(data_ptr_t data, extent_t extent, ivec_t stride)
    {
        image_view v;
        v._data_ptr = data;
        v._extent = extent;
        v._stride = stride;
        return v;
    }

    // unrolled accessors
public:
    using accessor_t::at;
    using accessor_t::operator();

    // members (protected because image derives from this)
protected:
    data_ptr_t _data_ptr = nullptr;
    extent_t _extent;
    ivec_t _stride; // NOTE: in bytes
};
}
