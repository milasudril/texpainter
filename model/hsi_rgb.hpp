//@	{
//@  "targets":[{"name":"hsi_rgb.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_MODEL_HSIRGB_HPP
#define TEXPAINTER_MODEL_HSIRGB_HPP

#include "./pixel.hpp"

#include <cmath>

namespace Texpainter::Model
{
	struct Hsi
	{
		float hue;
		float saturation;
		float intensity;
		float alpha;
	};

	Pixel toRgb(Hsi const& hsi)
	{
		auto tmp = [](Hsi const& hsi) {
			auto const h = static_cast<float>(6.0f * hsi.hue);
			auto const z = 1.0f - std::abs(std::fmod(h, 2.0f) - 1.0f);
			auto const c = 3.0f * hsi.intensity * hsi.saturation;
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
		auto const h = [](auto val) {
			auto const maxval = max(val);
			auto const minval = min(val);
			auto const c = maxval - minval;
			if(c == 0.0f) { return 0.0f; }

			if(maxval == val.red()) { return std::fmod((val.green() - val.blue()) / c, 6.0f); }

			if(maxval == val.green()) { return (val.blue() - val.red()) / c + 2.0f; }

			if(maxval == val.blue()) { return (val.red() - val.green()) / c + 4.0f; }

			return 0.0f;
		}(pixel);

		auto const s = (i == 0.0f) ? 0.0f : 1.0f - min(pixel) / i;
		return Hsi{h / 6.0f, s, i, pixel.alpha()};
	}
}


#endif