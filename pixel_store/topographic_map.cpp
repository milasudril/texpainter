//@	{"targets":[{"name":"topographic_map.o", "type":"object", "pkgconfig_libs":["OpenEXR"]}]}

#include "./topographic_map.hpp"

#include <OpenEXR/ImfInputFile.h>
#include <OpenEXR/ImfOutputFile.h>
#include <OpenEXR/ImfChannelList.h>
#include <OpenEXR/ImfArray.h>
#include <OpenEXR/ImfTestFile.h>
#include <OpenEXR/ImfHeader.h>
#include <OpenEXR/ImfFrameBuffer.h>

Texpainter::PixelStore::TopographicMap Texpainter::PixelStore::load(
    Enum::Empty<TopographicMap>, void* arg, detail::InputFileFactory make_input_file)
{
	auto src = make_input_file(arg);

	auto box = src.header().dataWindow();

	auto w = box.max.x - box.min.x + 1;
	auto h = box.max.y - box.min.y + 1;

	if(w > 65535 || h > 65535)
	{ throw std::runtime_error{std::string{"This image is too large."}}; }

	TopographicMap ret{static_cast<uint32_t>(w), static_cast<uint32_t>(h)};
	Imf::FrameBuffer fb;
	fb.insert("nx",
	          Imf::Slice{Imf::FLOAT,
	                     (char*)(ret.pixels().data()) + 0 * sizeof(float),
	                     sizeof(TopographyInfo),
	                     sizeof(TopographyInfo) * w});
	fb.insert("ny",
	          Imf::Slice{Imf::FLOAT,
	                     (char*)(ret.pixels().data()) + 1 * sizeof(float),
	                     sizeof(TopographyInfo),
	                     sizeof(TopographyInfo) * w});
	fb.insert("nz",
	          Imf::Slice{Imf::FLOAT,
	                     (char*)(ret.pixels().data()) + 2 * sizeof(float),
	                     sizeof(TopographyInfo),
	                     sizeof(TopographyInfo) * w});
	fb.insert("Y",
	          Imf::Slice{Imf::FLOAT,
	                     (char*)(ret.pixels().data()) + 3 * sizeof(float),
	                     sizeof(TopographyInfo),
	                     sizeof(TopographyInfo) * w});

	src.setFrameBuffer(fb);
	src.readPixels(box.min.y, box.max.y);

	return ret;
}

void Texpainter::PixelStore::store(Span2d<TopographyInfo const> pixels,
                                   void* arg,
                                   detail::OutputFileFactory make_output_file)
{
	Imf::Header header{static_cast<int>(pixels.width()), static_cast<int>(pixels.height())};
	header.channels().insert("nx", Imf::Channel{Imf::FLOAT});
	header.channels().insert("ny", Imf::Channel{Imf::FLOAT});
	header.channels().insert("nz", Imf::Channel{Imf::FLOAT});
	header.channels().insert("Y", Imf::Channel{Imf::FLOAT});

	Imf::FrameBuffer fb;
	fb.insert("nx",
	          Imf::Slice{Imf::FLOAT,
	                     (char*)(pixels.data()) + 0 * sizeof(float),
	                     sizeof(TopographyInfo),
	                     sizeof(TopographyInfo) * pixels.width()});
	fb.insert("ny",
	          Imf::Slice{Imf::FLOAT,
	                     (char*)(pixels.data()) + 1 * sizeof(float),
	                     sizeof(TopographyInfo),
	                     sizeof(TopographyInfo) * pixels.width()});
	fb.insert("nx",
	          Imf::Slice{Imf::FLOAT,
	                     (char*)(pixels.data()) + 2 * sizeof(float),
	                     sizeof(TopographyInfo),
	                     sizeof(TopographyInfo) * pixels.width()});
	fb.insert("Y",
	          Imf::Slice{Imf::FLOAT,
	                     (char*)(pixels.data()) + 3 * sizeof(float),
	                     sizeof(TopographyInfo),
	                     sizeof(TopographyInfo) * pixels.width()});

	auto dest = make_output_file(arg, header);
	dest.setFrameBuffer(fb);
	dest.writePixels(pixels.height());
}

bool Texpainter::PixelStore::fileValid(Enum::Empty<TopographicMap>, char const* filename)
{
	return Imf::isOpenExrFile(filename);
}
