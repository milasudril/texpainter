//@	{
//@  "targets":[{"name":"hsi_rgb.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_PIXELSTORE_HSIRGB_HPP
#define TEXPAINTER_PIXELSTORE_HSIRGB_HPP

#include "./pixel.hpp"
#include "utils/bidirectional_interpolation_table.hpp"

#include <cmath>

// #define direct

namespace Texpainter::PixelStore
{
	struct Hsi
	{
		float hue;
		float saturation;
		float intensity;
		float alpha;
	};

	namespace detail
	{
		constexpr float my_fmod(float a, float b) { return a - (b * int(a / b)); }

		constexpr std::array<std::pair<float, float>, 11> hue_points{
		    {{0.0f, 0.0f},
		     {1.25e-01f, 3.6586624e-02f},
		     {2.5e-01f, 1.2826073e-01f},  // Yellow
		     {3.75e-01f, 2.4137302e-01f},
		     {5.0e-01f, 3.3333334e-01f},          // Green
		     {5.83333333e-01f, 5.0e-01f},         // Turqoise
		     {6.66666667e-01f, 6.0e-01f},         // Skyblue
		     {0.75f, 6.5e-01f},                   // Blue
		     {8.33333333e-01f, 7.125e-01f},       // Violett
		     {9.16666746e-01f, 8.90000026e-01f},  // Pink
		     {1.0f, 1.0f}}};

		constexpr BidirectionalInterpolationTable hue_table{hue_points};

		constexpr auto wrapHue(float value)
		{
#ifdef direct
			return value;
#else
			return my_fmod(hue_table.input(value), 1.0f);
#endif
		}

		constexpr auto unwrapHue(float value)
		{
#ifdef direct
			return value;
#else
			return my_fmod(hue_table.output(value), 1.0f);
#endif
		}
	}


	constexpr Pixel toRgb(Hsi const& hsi)
	{
		auto tmp = [](Hsi const& hsi) {
			auto const h = static_cast<float>(6.0f * detail::unwrapHue(hsi.hue));
			auto const z = 1.0f - std::abs(detail::my_fmod(h, 2.0f) - 1.0f);
			auto const c = (3.0f * hsi.intensity * hsi.saturation) / (1.0f + z);
			auto const x = c * z;

			if(h < 1.0f) { return Pixel{c, x, 0.0f, 0.0f}; }
			if(h < 2.0f) { return Pixel{x, c, 0.0f, 0.0f}; }
			if(h < 3.0f) { return Pixel{0.0f, c, x, 0.0f}; }
			if(h < 4.0f) { return Pixel{0.0f, x, c, 0.0f}; }
			if(h < 5.0f) { return Pixel{x, 0.0f, c, 0.0f}; }
			if(h < 6.0f) { return Pixel{c, 0.0f, x, 0.0f}; }

			return Pixel{0.0f, 0.0f, 0.0f, 0.0f};
		}(hsi);

		auto const m = hsi.intensity * (1.0f - hsi.saturation);
		return tmp + Pixel{m, m, m, hsi.alpha};
	}

	constexpr Hsi toHsi(Pixel pixel)
	{
		auto const i = intensity(pixel) / 3.0f;
		auto h       = [](auto val) {
            auto const maxval = max(val);
            auto const minval = min(val);
            auto const c      = maxval - minval;
            if(c == 0.0f) { return 0.0f; }

            if(maxval == val.red()) { return (val.green() - val.blue()) / c; }

            if(maxval == val.green()) { return (val.blue() - val.red()) / c + 2.0f; }

            if(maxval == val.blue()) { return (val.red() - val.green()) / c + 4.0f; }

            return 0.0f;
		}(pixel);

		h = h < 0.0f ? h + 6.0f : h;

		auto const s = (i == 0.0f) ? 0.0f : 1.0f - min(pixel) / i;
		return Hsi{detail::wrapHue(h / 6.0f), s, i, pixel.alpha()};
	}
}


#endif