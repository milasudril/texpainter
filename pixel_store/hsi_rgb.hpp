//@	{
//@  "targets":[{"name":"hsi_rgb.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_PIXELSTORE_HSIRGB_HPP
#define TEXPAINTER_PIXELSTORE_HSIRGB_HPP

#include "./pixel.hpp"

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
		constexpr auto HueYellow = 1.0987067e-01f;
		constexpr auto HueGreen  = 4.0303028e-01f;
		constexpr auto HueBlue   = 5.8716667e-01f;

		constexpr auto wrapHue(float value)
		{
			if(value < HueYellow) { return 0.25f * value / HueYellow; }

			if(value < HueGreen)
			{ return 0.25f + 0.25f * (value - HueYellow) / (HueGreen - HueYellow); }

			if(value < HueBlue) { return 0.5f + 0.25f * (value - HueGreen) / (HueBlue - HueGreen); }

			return 0.75f + 0.25f * (value - HueBlue) / (1.0f - HueBlue);
		}

		constexpr auto unwrapHue(float value)
		{
			if(value < 0.25f) { return 4.0f * value * HueYellow; }

			if(value < 0.5f) { return HueYellow + 4.0f * (value - 0.25f) * (HueGreen - HueYellow); }

			if(value < 0.75f) { return HueGreen + 4.0f * (value - 0.5f) * (HueBlue - HueGreen); }

			return HueBlue + 4.0f * (value - 0.75f) * (1.0f - HueBlue);
		}
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