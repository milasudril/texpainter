//@	{"targets":[{"name":"grayscale_noise.hpp", "type":"include"}]}

#ifndef TEXPAINTER_GENERATORS_HPP
#define TEXPAINTER_GENERATORS_HPP

#include <random>
#include <algorithm>

namespace Texpainter::Generators
{
	template<class Rng = std::mt19937, class Distribution = std::uniform_real_distribution<float>>
	class GrayscaleNoise
	{
	public:
		explicit GrayscaleNoise(Rng&& rng = Rng{}, Distribution dist = Distribution{}): m_rng{std::move(rng)}, m_dist{dist}
		{
		}

		Model::BasicImage<float> operator()(std::tuple<uint32_t, uint32_t> size)
		{
			Model::BasicImage<float> ret{size};
			std::ranges::generate(ret.pixels(), [this]() {
				return m_dist(m_rng);
			});
			return ret;
		}

	private:
		Rng m_rng;
		Distribution m_dist;
	};
}

#endif