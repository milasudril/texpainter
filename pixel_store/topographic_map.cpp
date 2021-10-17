//@	{"targets":[{"name":"topographic_map.o", "type":"object", "pkgconfig_libs":["OpenEXR"]}]}

#include "./topographic_map.hpp"

#include <OpenEXR/ImfInputFile.h>
#include <OpenEXR/ImfOutputFile.h>
#include <OpenEXR/ImfChannelList.h>
#include <OpenEXR/ImfArray.h>
#include <OpenEXR/ImfTestFile.h>
#include <OpenEXR/ImfHeader.h>
#include <OpenEXR/ImfFrameBuffer.h>

void Texpainter::PixelStore::store(Span2d<TopographyInfo const> pixels,
                                   void* arg,
                                   detail::OutputFileFactory make_output_file)
{
	Imf::Header header{static_cast<int>(pixels.width()), static_cast<int>(pixels.height())};
	header.channels().insert("Y", Imf::Channel{Imf::FLOAT});

	Imf::FrameBuffer fb;
	fb.insert("Y",
	          Imf::Slice{Imf::FLOAT,
	                     (char*)(pixels.data()) + 3 * sizeof(float),
	                     sizeof(TopographyInfo),
	                     sizeof(TopographyInfo) * pixels.width()});

	auto dest = make_output_file(arg, header);
	dest.setFrameBuffer(fb);
	dest.writePixels(pixels.height());
}