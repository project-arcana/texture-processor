#pragma once

#include <clean-core/always_false.hh>
#include <clean-core/sentinel.hh>

#include <typed-geometry/tg-lean.hh>

#include <texture-processor/detail/predicates.hh>

namespace tp::detail
{
template <class iterator_t>
struct srange
{
    iterator_t it;
    srange(iterator_t it) : it(it) {}

    iterator_t begin() const { return it; }
    cc::sentinel end() const { return {}; }
};

// TODO: can probably be optimized further
template <int D>
struct strided_linear_pos_iterator
{
    strided_linear_pos_iterator(tg::vec<D, int> byte_stride, tg::vec<D, int> extent)
    {
        if constexpr (D == 1)
        {
            (void)byte_stride; // unused
            _extent[0] = extent.x;
        }
        else if constexpr (D == 2)
        {
            auto s0 = byte_stride.x >= 0 ? byte_stride.x : -byte_stride.x;
            auto s1 = byte_stride.y >= 0 ? byte_stride.y : -byte_stride.y;
            if (s0 <= s1)
            {
                _extent[0] = extent.x;
                _extent[1] = extent.y;
                _order[0] = 0;
                _order[1] = 1;
            }
            else
            {
                _extent[0] = extent.y;
                _extent[1] = extent.x;
                _order[0] = 1;
                _order[1] = 0;
            }
        }
        else
            static_assert(cc::always_false<D>, "dimension not supported / implemented");
    }

    tg::pos<D, int> operator*() const
    {
        if constexpr (D == 1)
            return {_idx[0]};
        else if constexpr (D == 2)
            return {_idx[_order[0]], _idx[_order[1]]};
        else if constexpr (D == 3)
            return {_idx[_order[0]], _idx[_order[1]], _idx[_order[2]]};
        else if constexpr (D == 4)
            return {_idx[_order[0]], _idx[_order[1]], _idx[_order[2]], _idx[_order[3]]};
        else
            static_assert(cc::always_false<D>, "dimension not supported");
    }

    void operator++()
    {
        ++_idx[0];

        if constexpr (D >= 2)
        {
            if (_idx[0] == _extent[0])
            {
                _idx[0] = 0;
                ++_idx[1];

                if constexpr (D >= 3)
                {
                    if (_idx[1] == _extent[1])
                    {
                        _idx[1] = 0;
                        ++_idx[2];

                        if constexpr (D >= 4)
                        {
                            if (_idx[2] == _extent[2])
                            {
                                _idx[2] = 0;
                                ++_idx[3];
                            }
                        }
                    }
                }
            }
        }
    }

    bool operator!=(cc::sentinel) const
    {
        if constexpr (D == 1)
            return _idx[0] != _extent[0];
        else if constexpr (D == 2)
            return _idx[0] != _extent[0] || _idx[1] != _extent[1];
        else if constexpr (D == 3)
            return _idx[0] != _extent[0] || _idx[1] != _extent[1] || _idx[2] != _extent[2];
        else if constexpr (D == 4)
            return _idx[0] != _extent[0] || _idx[1] != _extent[1] || _idx[2] != _extent[2] || _idx[3] != _extent[3];
        else
            static_assert(cc::always_false<D>, "dimension not supported");
    }

private:
    int _idx[D] = {};
    int _extent[D];
    [[maybe_unused]] uint8_t _order[D];
};

// TODO: can be optimized by sharing stride and extent with pos iterator
template <int D, class StorageViewT>
struct strided_linear_element_iterator
{
    using data_ptr_t = typename StorageViewT::data_ptr_t;

    strided_linear_element_iterator(data_ptr_t data, tg::vec<D, int> byte_stride, tg::vec<D, int> extent)
      : _pos_it(byte_stride, extent), _data(data), _byte_stride(byte_stride)
    {
    }

    typename StorageViewT::element_access_t operator*() const { return StorageViewT::element_at(_data, *_pos_it, _byte_stride); }
    void operator++() { _pos_it++; }
    bool operator!=(cc::sentinel) const { return _pos_it != cc::sentinel{}; }

private:
    strided_linear_pos_iterator<D> _pos_it;
    data_ptr_t _data;
    tg::vec<D, int> _byte_stride;
};
}
