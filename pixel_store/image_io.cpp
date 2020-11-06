//@	{"targets":[{"name":"image_io.o", "type":"object", "pkgconfig_libs":["OpenEXR"]}]}

#include "./image_io.hpp"

#include <OpenEXR/ImfInputFile.h>
#include <OpenEXR/ImfOutputFile.h>
#include <OpenEXR/ImfChannelList.h>
#include <OpenEXR/ImfArray.h>

Texpainter::PixelStore::Image Texpainter::PixelStore::load(const char* filename)
{
	Imf::InputFile src{filename};

	auto box = src.header().dataWindow();

	auto w = box.max.x - box.min.x + 1;
	auto h = box.max.y - box.min.y + 1;

	if(w > 65535 || h > 65535)
	{ throw std::runtime_error{std::string{"This image is too large."}}; }

	Image ret{static_cast<uint32_t>(w), static_cast<uint32_t>(h)};
	Imf::FrameBuffer fb;
	fb.insert("R",
	          Imf::Slice{Imf::FLOAT,
	                     (char*)(ret.pixels().data()) + 0 * sizeof(float),
	                     sizeof(Pixel),
	                     sizeof(Pixel) * w});
	fb.insert("G",
	          Imf::Slice{Imf::FLOAT,
	                     (char*)(ret.pixels().data()) + 1 * sizeof(float),
	                     sizeof(Pixel),
	                     sizeof(Pixel) * w});
	fb.insert("B",
	          Imf::Slice{Imf::FLOAT,
	                     (char*)(ret.pixels().data()) + 2 * sizeof(float),
	                     sizeof(Pixel),
	                     sizeof(Pixel) * w});
	fb.insert("A",
	          Imf::Slice{Imf::FLOAT,
	                     (char*)(ret.pixels().data()) + 3 * sizeof(float),
	                     sizeof(Pixel),
	                     sizeof(Pixel) * w});

	src.setFrameBuffer(fb);
	src.readPixels(box.min.y, box.max.y);

	return ret;
}

void Texpainter::PixelStore::store(Span2d<Pixel const> pixels, char const* filename)
{
	Imf::Header header{static_cast<int>(pixels.width()), static_cast<int>(pixels.height())};
	header.channels().insert("R", Imf::Channel{Imf::FLOAT});
	header.channels().insert("G", Imf::Channel{Imf::FLOAT});
	header.channels().insert("B", Imf::Channel{Imf::FLOAT});
	header.channels().insert("A", Imf::Channel{Imf::FLOAT});

	Imf::FrameBuffer fb;
	fb.insert("R",
	          Imf::Slice{Imf::FLOAT,
	                     (char*)(pixels.data()) + 0 * sizeof(float),
	                     sizeof(Pixel),
	                     sizeof(Pixel) * pixels.width()});
	fb.insert("G",
	          Imf::Slice{Imf::FLOAT,
	                     (char*)(pixels.data()) + 1 * sizeof(float),
	                     sizeof(Pixel),
	                     sizeof(Pixel) * pixels.width()});
	fb.insert("B",
	          Imf::Slice{Imf::FLOAT,
	                     (char*)(pixels.data()) + 2 * sizeof(float),
	                     sizeof(Pixel),
	                     sizeof(Pixel) * pixels.width()});
	fb.insert("A",
	          Imf::Slice{Imf::FLOAT,
	                     (char*)(pixels.data()) + 3 * sizeof(float),
	                     sizeof(Pixel),
	                     sizeof(Pixel) * pixels.width()});

	Imf::OutputFile dest{filename, header};
	dest.setFrameBuffer(fb);
	dest.writePixels(pixels.height());
}