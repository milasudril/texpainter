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
		constexpr auto HueUnwrappedYellow   = 1.0f / 6.0f;
		constexpr auto HueUnwrappedGreen    = 3.4527740e-01f;
		constexpr auto HueUnwrappedBlue     = 6.1349493e-01f;
		constexpr auto HueUnwrappedIndigo   = 6.9079286e-01f;
		constexpr auto HueUnwrappedViolette = 7.5191814e-01f;
		constexpr auto HueUnwrappedPink     = 9.2916709e-01f;

		constexpr auto HueWrappedYellow   = 4.0f / 12.0f;
		constexpr auto HueWrappedGreen    = 6.0f / 12.0f;
		constexpr auto HueWrappedBlue     = 8.0f / 12.0f;
		constexpr auto HueWrappedIndigo   = 9.0f / 12.0f;
		constexpr auto HueWrappedViolette = 10.0f / 12.0f;
		constexpr auto HueWrappedPink     = 11.0f / 12.0f;


		constexpr auto wrapHue(float value)
		{
			if(value < HueUnwrappedYellow) { return HueWrappedYellow * value / HueUnwrappedYellow; }

			if(value < HueUnwrappedGreen)
			{
				return HueWrappedYellow
				       + (HueWrappedGreen - HueWrappedYellow) * (value - HueUnwrappedYellow)
				             / (HueUnwrappedGreen - HueUnwrappedYellow);
			}

			if(value < HueUnwrappedBlue)
			{
				return HueWrappedGreen
				       + (HueWrappedBlue - HueWrappedGreen) * (value - HueUnwrappedGreen)
				             / (HueUnwrappedBlue - HueUnwrappedGreen);
			}

			if(value < HueUnwrappedIndigo)
			{
				return HueWrappedBlue
				       + (HueWrappedIndigo - HueWrappedBlue) * (value - HueUnwrappedBlue)
				             / (HueUnwrappedIndigo - HueUnwrappedBlue);
			}

			if(value < HueUnwrappedViolette)
			{
				return HueWrappedIndigo
				       + (HueWrappedViolette - HueWrappedIndigo) * (value - HueUnwrappedIndigo)
				             / (HueUnwrappedViolette - HueUnwrappedIndigo);
			}

			if(value < HueUnwrappedPink)
			{
				return HueWrappedViolette
				       + (HueWrappedPink - HueWrappedViolette) * (value - HueUnwrappedViolette)
				             / (HueUnwrappedPink - HueUnwrappedViolette);
			}

			return HueWrappedPink
			       + (1.0f - HueWrappedPink) * (value - HueUnwrappedPink)
			             / (1.0f - HueUnwrappedPink);
		}

		constexpr auto unwrapHue(float value)
		{
			if(value < HueWrappedYellow) { return value * HueUnwrappedYellow / HueWrappedYellow; }

			if(value < HueWrappedGreen)
			{
				return HueUnwrappedYellow
				       + (value - HueWrappedYellow) * (HueUnwrappedGreen - HueUnwrappedYellow)
				             / (HueWrappedGreen - HueWrappedYellow);
			}

			if(value < HueWrappedBlue)
			{
				return HueUnwrappedGreen
				       + (value - HueWrappedGreen) * (HueUnwrappedBlue - HueUnwrappedGreen)
				             / (HueWrappedBlue - HueWrappedGreen);
			}

			if(value < HueWrappedIndigo)
			{
				return HueUnwrappedBlue
				       + (value - HueWrappedBlue) * (HueUnwrappedIndigo - HueUnwrappedBlue)
				             / (HueWrappedIndigo - HueWrappedBlue);
			}

			if(value < HueWrappedViolette)
			{
				return HueUnwrappedIndigo
				       + (value - HueWrappedIndigo) * (HueUnwrappedViolette - HueUnwrappedIndigo)
				             / (HueWrappedViolette - HueWrappedIndigo);
			}

			if(value < HueWrappedPink)
			{
				return HueUnwrappedViolette
				       + (value - HueWrappedViolette) * (HueUnwrappedPink - HueUnwrappedViolette)
				             / (HueWrappedPink - HueWrappedViolette);
			}

			return HueUnwrappedPink
			       + (value - HueWrappedPink) * (1.0f - HueUnwrappedPink) / (1.0f - HueWrappedPink);
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