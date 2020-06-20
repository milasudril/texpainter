//@	{
//@	  "targets":[{"name":"surface_creator.hpp", "type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"surface_creator.o","rel":"implementation"}]
//@	 }

#include "generators/surface_generator.hpp"
#include "ui/labeled_input.hpp"
#include "ui/combobox.hpp"
#include "ui/slider.hpp"
#include "ui/image_view.hpp"
#include "ui/box.hpp"

#ifndef TEXPAINTER_SURFACECREATOR_HPP
#define TEXPAINTER_SURFACECREATOR_HPP

namespace Texpainter
{
	class SurfaceCreator
	{
	public:
		enum class ControlId : int
		{
			FilterGraph,
			Orientation,
			HorzCutoff,
			VertCutoff
		};

		explicit SurfaceCreator(Ui::Container& owner);

		template<ControlId id>
		void onChanged(Ui::Combobox&);

		template<ControlId id>
		void onChanged(Ui::Slider&);

	private:
		Model::Image m_preview;
		Generators::SurfaceGenerator m_generator; // TODO: should be a reference

		Ui::Box m_root;
		Ui::LabeledInput<Ui::Combobox> m_filter_graph;
		Ui::LabeledInput<Ui::Slider> m_orientation;
		Ui::LabeledInput<Ui::Slider> m_horz_cutoff;
		Ui::LabeledInput<Ui::Slider> m_vert_cutoff;
		Ui::ImageView m_img_view;
	};
}

#endif


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

		FilterGraph filters(FilterGraph f) const
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