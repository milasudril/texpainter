//@	{
//@	"targets":
//@		[{
//@		"name":"wavelength_to_rgba.o",
//@		"type":"object",
//@		"cxxoptions_local": {"cflags_extra":["fconstexpr-ops-limit=134217728"]}
//@		}]
//@	}

#include "./wavelength_to_rgba.hpp"

namespace
{
	constexpr auto gen_blackbody_colors()
	{
		constexpr auto N = 1024;

		std::array<Texpainter::PixelStore::RgbaValue, N> ret{};
		constexpr auto dT = 1.0 / N;
		for(size_t k = 0; k != std::size(ret); ++k)
		{
			ret[k] =
			    Texpainter::WavelengthConv::computeRgb([T = dT * static_cast<double>(k)](auto λ) {
				    return static_cast<float>(Texpainter::WavelengthConv::blackbodySpectrum(λ, T));
			    });
		}

		return ret;
	}

	constexpr auto bb_colors = gen_blackbody_colors();
}

Texpainter::PixelStore::RgbaValue Texpainter::WavelengthConv::blackbodyColor(float T)
{
	if(T < 0.0f) [[unlikely]]
		{
			return Texpainter::PixelStore::RgbaValue{0.0f, 0.0f, 0.0f, 0.0f};
		}

	auto const T_scaled = T * static_cast<float>(std::size(bb_colors) - 1);

	auto const index_low  = static_cast<size_t>(T_scaled);
	auto const index_high = index_low + 1;

	if(index_high >= std::size(bb_colors)) [[unlikely]]
		{
			//	auto ret = bb_colors.back();
			//	printf("%.7e %.7e %.7e\n", ret.red(), ret.green(), ret.blue());
			//	abort();
			return bb_colors.back();
		}

	auto const xi = T_scaled - static_cast<float>(index_low);

	//	printf("%.7e\n", T);
	//	printf("%.7e %.7e %.7e\n", bb_colors[index_low].red(), bb_colors[index_low].green(), bb_colors[index_low].blue());
	//	printf("%.7e %.7e %.7e\n", bb_colors[index_high].red(), bb_colors[index_high].green(), bb_colors[index_high].blue());
	//	abort();

	return xi * bb_colors[index_high] + (1.0f - xi) * bb_colors[index_low];
}