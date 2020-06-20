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
	enum class FilterGraph : int
	{
		None,
		Gaussian2d,
		Butt2d,
		Butt1d,
		Butt1dXThenY,
		Butt1dXYSum
	};

	template<FilterGraph id>
	struct FilterGraphItemTraits
	{
		static constexpr char const* name = "None";
	};

	template<>
	struct FilterGraphItemTraits<FilterGraph::Gaussian2d>
	{
		static constexpr char const* name = "2d gaussian";
	};

	template<>
	struct FilterGraphItemTraits<FilterGraph::Butt2d>
	{
		static constexpr char const* name = "2d 1st order Butterworth";
	};

	template<>
	struct FilterGraphItemTraits<FilterGraph::Butt1d>
	{
		static constexpr char const* name = "1d 1st order Butterworth";
	};

	template<>
	struct FilterGraphItemTraits<FilterGraph::Butt1dXThenY>
	{
		static constexpr char const* name = "1d 1st order Butterworth (x then y)";
	};

	template<>
	struct FilterGraphItemTraits<FilterGraph::Butt1dXYSum>
	{
		static constexpr char const* name = "1d 1st order Butterworth (x and y separated, then mixed)";
	};


	constexpr FilterGraph end(Empty<FilterGraph>)
	{
		return FilterGraph{static_cast<int>(FilterGraph::Butt1dXYSum) + 1};
	}

	constexpr FilterGraph begin(Empty<FilterGraph>)
	{
		return FilterGraph::None;
	}

	class SurfaceGenerator
	{
	public:
		SurfaceGenerator():
		   m_filters{FilterGraph::Butt2d},
		   m_orientation{0, Angle::Turns{}},
		   m_cutoff_freq{vec2_t{0.25, 0.5}}
		{
		}

		Model::Image operator()(Size2d output_size)
		{
			return select(m_filters, *this, output_size);
		}

		Model::Image operator()(Tag<FilterGraph::None>, Size2d);
		Model::Image operator()(Tag<FilterGraph::Butt2d>, Size2d);
		Model::Image operator()(Tag<FilterGraph::Gaussian2d>, Size2d);
		Model::Image operator()(Tag<FilterGraph::Butt1d>, Size2d);
		Model::Image operator()(Tag<FilterGraph::Butt1dXThenY>, Size2d);
		Model::Image operator()(Tag<FilterGraph::Butt1dXYSum>, Size2d);

		FilterGraph filters() const
		{
			return m_filters;
		}

		SurfaceGenerator& filters(FilterGraph f)
		{
			m_filters = f;
			return *this;
		}

		Angle orientation() const
		{
			return m_orientation;
		}

		SurfaceGenerator& orientation(Angle orientation)
		{
			m_orientation = orientation;
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