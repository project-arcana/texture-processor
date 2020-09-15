#include "io.hh"

#include <babel-serializer/file.hh>
#include <babel-serializer/image/image.hh>

bool tp::detail::write_to_file(cc::string_view filename, cc::string_view ext, cc::span<const std::byte> data, int w, int h, int channels, int bit_depth)
{
    CC_ASSERT(1 <= channels && channels <= 4);
    babel::image::data_header header;
    header.width = w;
    header.height = h;
    header.channels = (babel::image::channels)channels;
    header.depth = 1;
    switch (bit_depth)
    {
    case 8:
        header.bit_depth = babel::image::bit_depth::u8;
        break;
    case 16:
        header.bit_depth = babel::image::bit_depth::u16;
        break;
    case 32:
        header.bit_depth = babel::image::bit_depth::f32;
        break;
    default:
        CC_UNREACHABLE("not supported");
    }

    return babel::image::write(babel::file::file_output_stream(filename), header, data, {ext});
}
