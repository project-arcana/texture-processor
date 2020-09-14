#pragma once

#include <cstdint>

namespace tp
{
/// 4x4 block of 64bit
struct blockDXT1
{
    static constexpr int block_sizes[] = {4, 4};

    uint64_t data = 0;
};
}
