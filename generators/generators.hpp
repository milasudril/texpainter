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
			explicit GrayscaleNoise(Rng& rng, Distribution dist = Distribution{}):r_rng{rng},m_dist{dist}{}

			std::span<Model::Pixel> operator|(std::span<Model::Pixel> pixels)
			{
				std::ranges::generate(pixels, [this]()
				{
					auto val = m_dist(r_rng);
					return Model::Pixel{val, val, val, 1.0f};
				});

				return pixels;
			}

		private:
			Rng& r_rng;
			Distribution m_dist;
	};
}

#endif