//@	{
//@	 "targets":[{"name":"image_io.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"image_io.o", "rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_PIXELSTORE_IMAGEIO_HPP
#define TEXPAINTER_PIXELSTORE_IMAGEIO_HPP

#include "./image.hpp"
#include "./ilm_io.hpp"

#include "libenum/empty.hpp"

namespace Texpainter::PixelStore
{
	Image load(Enum::Empty<Image>, char const* filename);

	bool fileValid(Enum::Empty<Image>, char const* filename);

	void store(Span2d<Pixel const> pixels, char const* filename);

	inline void store(Image const& img, char const* filename) { store(img.pixels(), filename); }

	void store(Span2d<Pixel const> pixels, IlmOutputAdapter&& ilm_output);

	template<class FileWriter>
	void store(Image const& img, FileWriter writer)
	{
		store(img.pixels(), IlmOutputAdapter{std::ref(writer)});
	}
}

#endif