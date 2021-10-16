//@	{
//@	 "targets":[{"name":"rgba_image.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"rgba_image.o", "rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_PIXELSTORE_RGBAIMAGE_HPP
#define TEXPAINTER_PIXELSTORE_RGBAIMAGE_HPP

#include "./rgba_value.hpp"
#include "./ilm_io.hpp"

#include "pixel_store/image.hpp"

namespace Texpainter::PixelStore
{
	using RgbaImage = PixelStore::Image<RgbaValue>;

	RgbaImage load(Enum::Empty<RgbaImage>, void* arg, detail::InputFileFactory make_input_file);

	inline RgbaImage load(Enum::Empty<RgbaImage>, char const* filename)
	{
		return load(Enum::Empty<RgbaImage>{}, const_cast<char*>(filename), [](void* filename) {
			return Imf::InputFile{static_cast<char const*>(filename)};
		});
	}

	template<class FileReader>
	RgbaImage load(Enum::Empty<RgbaImage>, FileReader&& reader)
	{
		IlmInputAdapter input{std::forward<FileReader>(reader)};
		return load(Enum::Empty<RgbaImage>{}, &input, [](void* input) {
			return Imf::InputFile{*static_cast<IlmInputAdapter<FileReader>*>(input)};
		});
	}

	bool fileValid(Enum::Empty<RgbaImage>, char const* filename);


	void store(Span2d<RgbaValue const> pixels,
	           void* arg,
	           detail::OutputFileFactory make_output_file);

	inline void store(Span2d<RgbaValue const> pixels, char const* filename)
	{
		store(pixels, const_cast<char*>(filename), [](void* filename, Imf::Header const& header) {
			return Imf::OutputFile{static_cast<char const*>(filename), header};
		});
	}

	inline void store(RgbaImage const& img, char const* filename) { store(img.pixels(), filename); }

	template<class FileWriter>
	void store(Span2d<RgbaValue const> pixels, FileWriter&& writer)
	{
		IlmOutputAdapter output{std::forward<FileWriter>(writer)};
		store(pixels, &output, [](void* output, Imf::Header const& header) {
			return Imf::OutputFile{*static_cast<IlmOutputAdapter<FileWriter>*>(output), header};
		});
	}

	template<class FileWriter>
	void store(RgbaImage const& img, FileWriter&& writer)
	{
		store(img.pixels(), std::forward<FileWriter>(writer));
	}
}

#endif