//@	{
//@	 "targets":[{"name":"palette_generate.o","type":"object"}]
//@	 ,"dependencies_extra":[{"ref":"palette_generate.o","rel":"implementation"}]
//@	}

#include "./palette_generate.hpp"

#include "pixel_store/hsi_rgb.hpp"

Texpainter::PixelStore::Palette<16> Texpainter::Model::generatePalette(
    std::array<PixelStore::Pixel, 4> const& base_colors)
{
	Texpainter::PixelStore::Palette<16> ret;
	constexpr auto IntensityLevels      = 3;
	constexpr auto IntensityScaleFactor = 2.0f;
	std::ranges::for_each(base_colors, [&ret, k = 0u](auto val) mutable {
		auto hsi = toHsi(val);
		for(int l = 0; l < IntensityLevels; ++l)
		{
			ret[PixelStore::ColorIndex{k}] = toRgb(hsi);
			ret[PixelStore::ColorIndex{k}].alpha(hsi.intensity);
			++k;
			hsi.intensity /= IntensityScaleFactor;
		}
	});

	auto i = 1.0f;
	for(int k = 0; k < IntensityLevels; ++k)
	{
		PixelStore::ColorIndex const index{
		    IntensityLevels * static_cast<uint32_t>(base_colors.size()) + k};
		ret[index] = PixelStore::Pixel{1.0f, 1.0f, 1.0f, 1.0f} / (3.0f * i);
		i /= IntensityScaleFactor;
	}
	return ret;
}