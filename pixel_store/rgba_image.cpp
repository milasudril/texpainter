//@	{"targets":[{"name":"rgba_image.o", "type":"object", "pkgconfig_libs":["OpenEXR"]}]}

#include "./rgba_image.hpp"

#include <OpenEXR/ImfInputFile.h>
#include <OpenEXR/ImfOutputFile.h>
#include <OpenEXR/ImfChannelList.h>
#include <OpenEXR/ImfArray.h>
#include <OpenEXR/ImfTestFile.h>
#include <OpenEXR/ImfHeader.h>
#include <OpenEXR/ImfFrameBuffer.h>

Texpainter::PixelStore::RgbaImage Texpainter::PixelStore::load(
    Enum::Empty<RgbaImage>, void* arg, detail::InputFileFactory make_input_file)
{
	auto src = make_input_file(arg);

	auto box = src.header().dataWindow();

	auto w = box.max.x - box.min.x + 1;
	auto h = box.max.y - box.min.y + 1;

	if(w > 65535 || h > 65535)
	{ throw std::runtime_error{std::string{"This image is too large."}}; }

	RgbaImage ret{static_cast<uint32_t>(w), static_cast<uint32_t>(h)};
	Imf::FrameBuffer fb;
	fb.insert("R",
	          Imf::Slice{Imf::FLOAT,
	                     (char*)(ret.pixels().data()) + 0 * sizeof(float),
	                     sizeof(RgbaValue),
	                     sizeof(RgbaValue) * w});
	fb.insert("G",
	          Imf::Slice{Imf::FLOAT,
	                     (char*)(ret.pixels().data()) + 1 * sizeof(float),
	                     sizeof(RgbaValue),
	                     sizeof(RgbaValue) * w});
	fb.insert("B",
	          Imf::Slice{Imf::FLOAT,
	                     (char*)(ret.pixels().data()) + 2 * sizeof(float),
	                     sizeof(RgbaValue),
	                     sizeof(RgbaValue) * w});
	fb.insert("A",
	          Imf::Slice{Imf::FLOAT,
	                     (char*)(ret.pixels().data()) + 3 * sizeof(float),
	                     sizeof(RgbaValue),
	                     sizeof(RgbaValue) * w});

	src.setFrameBuffer(fb);
	src.readPixels(box.min.y, box.max.y);

	if(!src.header().channels().findChannel("A"))
	{
		std::ranges::transform(ret.pixels(), std::begin(ret.pixels()), [](auto val) {
			val.alpha(1.0f);
			return val;
		});
	}

	return ret;
}

void Texpainter::PixelStore::store(Span2d<RgbaValue const> pixels,
                                   void* arg,
                                   detail::OutputFileFactory make_output_file)
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
	                     sizeof(RgbaValue),
	                     sizeof(RgbaValue) * pixels.width()});
	fb.insert("G",
	          Imf::Slice{Imf::FLOAT,
	                     (char*)(pixels.data()) + 1 * sizeof(float),
	                     sizeof(RgbaValue),
	                     sizeof(RgbaValue) * pixels.width()});
	fb.insert("B",
	          Imf::Slice{Imf::FLOAT,
	                     (char*)(pixels.data()) + 2 * sizeof(float),
	                     sizeof(RgbaValue),
	                     sizeof(RgbaValue) * pixels.width()});
	fb.insert("A",
	          Imf::Slice{Imf::FLOAT,
	                     (char*)(pixels.data()) + 3 * sizeof(float),
	                     sizeof(RgbaValue),
	                     sizeof(RgbaValue) * pixels.width()});

	auto dest = make_output_file(arg, header);
	dest.setFrameBuffer(fb);
	dest.writePixels(pixels.height());
}

bool Texpainter::PixelStore::fileValid(Enum::Empty<RgbaImage>, char const* filename)
{
	return Imf::isOpenExrFile(filename);
}
