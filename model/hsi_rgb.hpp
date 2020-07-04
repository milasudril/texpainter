//@	{
//@  "targets":[{"name":"hsi_rgb.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_MODEL_HSIRGB_HPP
#define TEXPAINTER_MODEL_HSIRGB_HPP

#include "./pixel.hpp"

#include "utils/angle.hpp"

#include <cmath>

namespace Texpainter::Model
{
	struct Hsi
	{
		Angle hue;
		float saturation;
		float intensity;
		float alpha;
	};

	Pixel convert(Hsi const& hsi)
	{

		auto tmp = [](Hsi const& hsi){
			auto const h = 6.0f*hsi.hue.turns();
			auto const z = 1.0f - std::abs(std::fmod(h, 2.0f) - 1.0f);
			auto const c = 3.0f * hsi.intensity*hsi.saturation;
			auto const x = c*z;
			if(h < 1.0f) {return Pixel{c, x, 0.0f, 0.0f};}
			if(h < 2.0f) {return Pixel{x, c, 0.0f, 0.0f};}
			if(h < 3.0f) {return Pixel{0.0f, c, x, 0.0f};}
			if(h < 4.0f) {return Pixel{0.0f, x, c, 0.0f};}
			if(h < 5.0f) {return Pixel{x, 0.0f, c, 0.0f};}
			if(h < 6.0f) {return Pixel{c, 0.0f, x, 0.0f};}
			return Pixel{0.0f, 0.0f, 0.0f, a};
		}(hsi);

		auto const m = hsi.intensity*(1.0f - hsi.saturation);
		return tmp + Pixel{m, m, m, hsi.alpha};
	}

	Hsi convert(Pixel pixel)
	{
	}
}


#endif