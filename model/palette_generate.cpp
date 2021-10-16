//@	{
//@	 "targets":[{"name":"palette_generate.o","type":"object"}]
//@	 ,"dependencies_extra":[{"ref":"palette_generate.o","rel":"implementation"}]
//@	}

#include "./palette_generate.hpp"

#include "pixel_store/hsi_rgb.hpp"

namespace
{
	constexpr auto IntensityLevels = 3;
}

namespace
{
	Texpainter::Model::Palette generatePaletteByHue(
	    std::array<Texpainter::PixelStore::RgbaValue, 4> const& base_colors,
	    bool intensity_to_intensity,
	    bool intensity_to_alpha)
	{
		Texpainter::Model::Palette ret;
		constexpr auto IntensityScaleFactor = 2.0f;
		std::ranges::for_each(base_colors,
		                      [&ret,
		                       k = Texpainter::PixelStore::ColorIndex{0},
		                       intensity_to_intensity,
		                       intensity_to_alpha](auto val) mutable {
			                      auto hsi = toHsi(val);
			                      for(int l = 0; l < IntensityLevels; ++l)
			                      {
				                      ret[k] = toRgb(hsi);
				                      ++k;
				                      if(intensity_to_intensity)
				                      { hsi.intensity /= IntensityScaleFactor; }

				                      if(intensity_to_alpha) { hsi.alpha /= IntensityScaleFactor; }
			                      }
		                      });

		auto i = 1.0f;
		for(int k = 0; k < IntensityLevels; ++k)
		{
			Texpainter::PixelStore::ColorIndex const index{
			    static_cast<Texpainter::PixelStore::ColorIndex::element_type>(
			        IntensityLevels * base_colors.size() + k)};
			ret[index] =
			    intensity_to_alpha
			        ? Texpainter::PixelStore::RgbaValue{1.0f, 1.0f, 1.0f, 3.0f} / (3.0f * i)
			        : Texpainter::PixelStore::RgbaValue{1.0f, 1.0f, 1.0f, 0.0f} / (3.0f * i)
			              + Texpainter::PixelStore::RgbaValue{0.0, 0.0, 0.0, 1.0};
			i *= IntensityScaleFactor;
		}
		return ret;
	}

	Texpainter::Model::Palette orderByIntensity(Texpainter::Model::Palette const& pal)
	{
		Texpainter::Model::Palette ret;

		auto constexpr NumColors = 5;

		for(size_t k = 0; k < NumColors; ++k)
		{
			for(size_t l = 0; l < static_cast<size_t>(IntensityLevels); ++l)
			{
				auto const src_index = Texpainter::PixelStore::ColorIndex{
				    static_cast<Texpainter::PixelStore::ColorIndex::element_type>(
				        k * IntensityLevels + l)};
				auto const dest_index = Texpainter::PixelStore::ColorIndex{
				    static_cast<Texpainter::PixelStore::ColorIndex::element_type>(l * NumColors
				                                                                  + k)};
				ret[dest_index] = pal[src_index];
			}
		}
		return ret;
	}
}


Texpainter::Model::Palette Texpainter::Model::generatePalette(PaletteParameters const& params)
{
	auto ret = generatePaletteByHue(
	    params.colors, params.intensity_to_intensity, params.intensity_to_alpha);

	if(params.by_intensity) { ret = orderByIntensity(ret); }

	if(params.reversed) { std::ranges::reverse(ret); }

	return ret;
}