//@	{
//@  "targets":[{"name":"hsi_rgb.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_PIXELSTORE_HSIRGB_HPP
#define TEXPAINTER_PIXELSTORE_HSIRGB_HPP

#include "./pixel.hpp"
#include "utils/bidirectional_interpolation_table.hpp"

#include <cmath>

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
		constexpr std::array<std::pair<float, float>, 9> hue_points{
		    {{0.0f, 0.0f},
		     {0.125f, 3.6586624e-02f},
		     {0.25f, 1.2826073e-01f},  // Yellow
		     {0.375f, 2.4137302e-01f},
		     {0.5f, 3.3333334e-01f},  // Green
		     {0.625f, 0.416f},
		     {0.75f, 0.625f},  // Blue
		     {0.875f, 0.8125f},
		     {1.0f, 1.0f}}};

		constexpr BidirectionalInterpolationTable hue_table{hue_points};

		constexpr auto wrapHue(float value) { return std::fmod(hue_table.input(value), 1.0f); }

		constexpr auto unwrapHue(float value) { return std::fmod(hue_table.output(value), 1.0f); }
	}


	inline Pixel toRgb(Hsi const& hsi)
	{
		auto tmp = [](Hsi const& hsi) {
			auto const h = static_cast<float>(6.0f * detail::unwrapHue(hsi.hue));
			auto const z = 1.0f - std::abs(std::fmod(h, 2.0f) - 1.0f);
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

	inline Hsi toHsi(Pixel pixel)
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