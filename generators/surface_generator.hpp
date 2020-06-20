//@	{
//@	  "targets":[{"name":"surface_generator.hpp", "type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"surface_generator.o", "rel":"implementation"}]
//@	 }

#ifndef TEXPAINTER_SURFACEGENERATOR_HPP
#define TEXPAINTER_SURFACEGENERATOR_HPP

#include "generators/fourier_transform.hpp"
#include "generators/grayscale_noise.hpp"
#include "model/image.hpp"
#include "utils/angle.hpp"
#include "utils/frequency.hpp"

namespace Texpainter::Generators
{
	class SurfaceGenerator
	{
	public:
		enum class FilterGraph : int
		{
			Butt2d,
			Gaussian2d,
			Butt1d,
			Butt1dXThenY,
			Butt1dXYSum
		};

		SurfaceGenerator():
		   m_filters{FilterGraph::Butt2d},
		   m_orientation{0, Angle::Turns{}},
		   m_cutoff_freq{vec2_t{0.25, 0.5}}
		{
		}

		Model::Image operator()(Size2d output_size);

		Angle orientation() const
		{
			return m_orientation;
		}

		SurfaceGenerator& orientation(Angle orientation)
		{
			m_orientation = m_orientation;
			return *this;
		}

		SpatialFrequency cutoffFrequency() const
		{
			return m_cutoff_freq;
		}

		SurfaceGenerator& cutoffFrequency(SpatialFrequency f)
		{
			m_cutoff_freq = f;
			return *this;
		}

		SurfaceGenerator& noiseRng(GrayscaleNoise<>::RandomGenerator&& rng)
		{
			m_noise.rngState(std::move(rng));
			return *this;
		}

		SurfaceGenerator& noiseDistribution(GrayscaleNoise<>::Distribution&& dist)
		{
			m_noise.probDist(std::move(dist));
			return *this;
		}


	private:
		FilterGraph m_filters;
		Angle m_orientation;
		SpatialFrequency m_cutoff_freq;
		GrayscaleNoise<> m_noise;
		FourierTransform m_fft;
	};
}

#endif