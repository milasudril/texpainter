//@	{"targets":[{"name":"grayscale_noise.hpp", "type":"include"}]}

#ifndef TEXPAINTER_GENERATORS_GRAYSCALENOISE_HPP
#define TEXPAINTER_GENERATORS_GRAYSCALENOISE_HPP

#include <random>
#include <algorithm>

namespace Texpainter::Generators
{
	template<class Distribution = std::uniform_real_distribution<float>, class Rng = std::mt19937>
	class GrayscaleNoise
	{
	public:
		explicit GrayscaleNoise(Distribution dist = Distribution{}, Rng&& rng = Rng{}):
		   m_rng{std::move(rng)},
		   m_dist{dist}
		{
		}

		Model::BasicImage<float> operator()(std::tuple<uint32_t, uint32_t> size)
		{
			Model::BasicImage<float> ret{size};
			std::ranges::generate(ret.pixels(), [this]() { return m_dist(m_rng); });
			return ret;
		}

	private:
		Rng m_rng;
		Distribution m_dist;
	};
}

#endif