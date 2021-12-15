//@	{
//@	"targets":
//@		[{
//@		"name":"wavelength_to_rgba.o",
//@		"type":"object",
//@		"cxxoptions_local": {"cflags_extra":["fconstexpr-ops-limit=67108864"]}
//@		}]
//@	}

#include "./wavelength_to_rgba.hpp"

namespace
{
	constexpr auto gen_blackbody_colors()
	{
		constexpr auto N  = 512;

		std::array<Texpainter::PixelStore::RgbaValue, N> ret{};
		constexpr auto dT = 1.0/N;
		for(size_t k = 0; k != std::size(ret); ++k)
		{
			ret[k] = Texpainter::WavelengthConv::computeRgb([T = dT*static_cast<double>(k)](auto λ) {
				return static_cast<float>(Texpainter::WavelengthConv::blackbodySpectrum(λ, T));
			});
		}

		return ret;
	}

	constexpr auto bb_colors = gen_blackbody_colors();
}

Texpainter::PixelStore::RgbaValue blackbodyColor(float)
{
	return Texpainter::PixelStore::RgbaValue{0.0f, 0.0f, 0.0f, 0.0f};
}