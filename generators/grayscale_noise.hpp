//@	{"targets":[{"name":"grayscale_noise.hpp", "type":"include"}]}

#ifndef TEXPAINTER_GENERATORS_GRAYSCALENOISE_HPP
#define TEXPAINTER_GENERATORS_GRAYSCALENOISE_HPP

#include "utils/size_2d.hpp"
#include "model/image.hpp"

#include <random>
#include <algorithm>

namespace Texpainter::Generators
{
	template<class Dist = std::uniform_real_distribution<double>, class Rng = std::mt19937>
	class GrayscaleNoise
	{
	public:
		using RandomGenerator = Rng;
		using Distribution = Dist;

		explicit GrayscaleNoise(Distribution dist = Distribution{}, Rng&& rng = Rng{}):
		   m_rng{std::move(rng)},
		   m_dist{dist}
		{
		}

		GrayscaleNoise& rngState(Rng&& state)
		{
			m_rng = std::move(state);
			return *this;
		}

		GrayscaleNoise& probDist(Distribution&& dist)
		{
			m_dist = std::move(dist);
			return *this;
		}

		Model::BasicImage<double> operator()(Size2d size)
		{
			Model::BasicImage<double> ret{size};
			std::ranges::generate(ret.pixels(), [this]() { return m_dist(m_rng); });
			return ret;
		}

	private:
		Rng m_rng;
		Distribution m_dist;
	};
}

#endif