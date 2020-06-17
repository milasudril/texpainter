//@	{"targets":[{"name":"generators.hpp", "type":"include"}]}

#ifndef TEXPAINTER_GENERATORS_HPP
#define TEXPAINTER_GENERATORS_HPP

#include "model/pixel.hpp"

#include <random>
#include <algorithm>

namespace Texpainter::Generators
{
	template<class Rng, class Distribution = std::uniform_real_distribution<float>>
	class GrayscaleNoise
	{
	public:
		explicit GrayscaleNoise(Rng& rng, Distribution dist = Distribution{}): r_rng{rng}, m_dist{dist}
		{
		}

		void operator()(Span2d<Model::Pixel> pixels)
		{
			std::ranges::generate(pixels, [this]() {
				auto val = m_dist(r_rng);
				return Model::Pixel{val, val, val, 1.0f};
			});
		}

	private:
		Rng& r_rng;
		Distribution m_dist;
	};

	class TestPattern
	{
	public:
		void operator()(Span2d<Model::Pixel> pixels) const
		{
			for(uint32_t row = 0; row < pixels.height(); ++row)
			{
				for(uint32_t col = 0; col < pixels.width(); ++col)
				{
					auto factors = Model::Pixel{
					   static_cast<float>(pixels.width()), static_cast<float>(pixels.height()), 1.0f, 1.0f};
					pixels(col, row) =
					   Model::Pixel{static_cast<float>(col), static_cast<float>(row), 0.0f, 1.0f} / factors;
				}
			}
		}
	};
}

#endif