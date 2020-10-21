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

Texpainter::PixelStore::Palette<16> Texpainter::Model::generatePaletteByHue(
    std::array<PixelStore::Pixel, 4> const& base_colors)
{
	Texpainter::PixelStore::Palette<16> ret;
	constexpr auto IntensityScaleFactor = 2.0f;
	std::ranges::for_each(base_colors, [&ret, k = 0u](auto val) mutable {
		auto hsi = toHsi(val);
		for(int l = 0; l < IntensityLevels; ++l)
		{
			ret[PixelStore::ColorIndex{k}] = toRgb(hsi);
			++k;
			hsi.intensity /= IntensityScaleFactor;
			hsi.alpha /= IntensityScaleFactor;
		}
	});

	auto i = 1.0f;
	for(int k = 0; k < IntensityLevels; ++k)
	{
		PixelStore::ColorIndex const index{
		    IntensityLevels * static_cast<uint32_t>(base_colors.size()) + k};
		ret[index] = PixelStore::Pixel{1.0f, 1.0f, 1.0f, 3.0f} / (3.0f * i);
		i *= IntensityScaleFactor;
	}
	return ret;
}

Texpainter::PixelStore::Palette<16> Texpainter::Model::generatePaletteByIntensity(
    std::array<PixelStore::Pixel, 4> const& base_colors)
{
	auto tmp = generatePaletteByHue(base_colors);
	Texpainter::PixelStore::Palette<16> ret;

	auto const NumColors = static_cast<uint32_t>(base_colors.size() + 1);  // +1 beacuse gray tones

	for(uint32_t k = 0; k < NumColors; ++k)
	{
		for(uint32_t l = 0; l < static_cast<uint32_t>(IntensityLevels); ++l)
		{
			auto const src_index  = PixelStore::ColorIndex{k * IntensityLevels + l};
			auto const dest_index = PixelStore::ColorIndex{l * NumColors + k};
			ret[dest_index]       = tmp[src_index];
		}
	}
	return ret;
}