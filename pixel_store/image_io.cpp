//@	{"targets":[{"name":"image_io.o", "type":"object", "pkgconfig_libs":["OpenEXR"]}]}

#include "./image_io.hpp"

#include <OpenEXR/ImfRgbaFile.h>
#include <OpenEXR/ImfArray.h>

Texpainter::PixelStore::Image Texpainter::PixelStore::load(const char* filename)
{
	Imf_2_3::RgbaInputFile src{filename};
	auto box = src.dataWindow();
	auto w   = box.max.x - box.min.x + 1;
	auto h   = box.max.y - box.min.y + 1;

	if(w > 65535 || h > 65535)
	{ throw std::runtime_error{std::string{"This image is too large."}}; }

	Imf_2_3::Array2D<Imf_2_3::Rgba> temp{w, h};

	src.setFrameBuffer(&temp[0][0], 1, w);
	src.readPixels(box.min.y, box.max.y);

	Image ret{static_cast<uint32_t>(w), static_cast<uint32_t>(h)};
	std::transform(&temp[0][0], &temp[0][0] + ret.area(), ret.pixels().data(), [](auto in) {
		return Pixel{in.r, in.g, in.b, in.a};
	});

	return ret;
}