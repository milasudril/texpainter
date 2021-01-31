//@	{
//@	 "targets":[{"name":"image_io.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"image_io.o", "rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_PIXELSTORE_IMAGEIO_HPP
#define TEXPAINTER_PIXELSTORE_IMAGEIO_HPP

#include "./image.hpp"
#include "./ilm_io.hpp"

#include "libenum/empty.hpp"

#include <OpenEXR/ImfOutputFile.h>

namespace Texpainter::PixelStore
{
	Image load(Enum::Empty<Image>, char const* filename);

	bool fileValid(Enum::Empty<Image>, char const* filename);

	namespace detail
	{
		using OutputFileFactory = Imf::OutputFile (*)(void*, Imf::Header const&);
	}

	void store(Span2d<Pixel const> pixels, void* arg, detail::OutputFileFactory make_output_file);

	inline void store(Span2d<Pixel const> pixels, char const* filename)
	{
		store(pixels, const_cast<char*>(filename), [](void* filename, Imf::Header const& header)
		{
			return Imf::OutputFile{static_cast<char const*>(filename), header};
		});
	}

	inline void store(Image const& img, char const* filename) { store(img.pixels(), filename); }

	void store(Span2d<Pixel const> pixels, IlmOutputAdapter&& ilm_output);

	template<class FileWriter>
	void store(Image const& img, FileWriter writer)
	{
		IlmOutputAdapter output{std::ref(writer)};
		store(img.pixels(), &output, [](void* output, Imf::Header const& header) {
			return Imf::OutputFile{*static_cast<IlmOutputAdapter*>(output), header};
		});
	}

	void store(Span2d<Pixel const> pixels, IlmOutputAdapter&& ilm_output);
}

#endif