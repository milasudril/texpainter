//@	{
//@	 "targets":[{"name":"topographic_map.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"topographic_map.o", "rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_PIXELSTORE_TOPOGRAPHICMAP_HPP
#define TEXPAINTER_PIXELSTORE_TOPOGRAPHICMAP_HPP

#include "./topography_info.hpp"
#include "./ilm_io.hpp"

#include "pixel_store/image.hpp"

namespace Texpainter::PixelStore
{
	using TopographicMap = Image<TopographyInfo>;

	TopographicMap load(Enum::Empty<TopographicMap>, void* arg, detail::InputFileFactory make_input_file);

	inline TopographicMap load(Enum::Empty<TopographicMap>, char const* filename)
	{
		return load(Enum::Empty<TopographicMap>{}, const_cast<char*>(filename), [](void* filename) {
			return Imf::InputFile{static_cast<char const*>(filename)};
		});
	}

	template<class FileReader>
	TopographicMap load(Enum::Empty<TopographicMap>, FileReader&& reader)
	{
		IlmInputAdapter input{std::forward<FileReader>(reader)};
		return load(Enum::Empty<TopographicMap>{}, &input, [](void* input) {
			return Imf::InputFile{*static_cast<IlmInputAdapter<FileReader>*>(input)};
		});
	}

	bool fileValid(Enum::Empty<TopographicMap>, char const* filename);


	void store(Span2d<TopographyInfo const> pixels,
	           void* arg,
	           detail::OutputFileFactory make_output_file);

	inline void store(Span2d<TopographyInfo const> pixels, char const* filename)
	{
		store(pixels, const_cast<char*>(filename), [](void* filename, Imf::Header const& header) {
			return Imf::OutputFile{static_cast<char const*>(filename), header};
		});
	}

	inline void store(TopographicMap const& img, char const* filename) { store(img.pixels(), filename); }

	template<class FileWriter>
	void store(Span2d<TopographyInfo const> pixels, FileWriter&& writer)
	{
		IlmOutputAdapter output{std::forward<FileWriter>(writer)};
		store(pixels, &output, [](void* output, Imf::Header const& header) {
			return Imf::OutputFile{*static_cast<IlmOutputAdapter<FileWriter>*>(output), header};
		});
	}

	template<class FileWriter>
	void store(TopographicMap const& img, FileWriter&& writer)
	{
		store(img.pixels(), std::forward<FileWriter>(writer));
	}
}

#endif