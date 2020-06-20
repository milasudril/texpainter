//@	{
//@	  "targets":[{"name":"surface_generator.o", "type":"object"}]
//@	 }

#include "./surface_generator.hpp"
#include "./butterworth_freq_2d.hpp"

namespace
{
	Texpainter::Model::Image normalizedRgba(Texpainter::Span2d<float const> source)
	{
		auto res = std::ranges::minmax_element(source);
		Texpainter::Model::Image ret{source.size()};
		std::ranges::transform(source, ret.pixels().begin(), [min = *res.min, max = *res.max](auto val) {
			Texpainter::Model::Pixel offset{-min, -min, -min, 0.0f};
			Texpainter::Model::Pixel factor{max - min, max - min, max - min, 1.0f};
			return (Texpainter::Model::Pixel{val, val, val, 1.0f} + offset) / factor;
		});
		return ret;
	}
}

Texpainter::Model::Image Texpainter::Generators::SurfaceGenerator::
operator()(Tag<FilterGraph::Butt2d>, Size2d size)
{
	auto freq_scaled = m_cutoff_freq * vec2_t{0.5 * size.width(), 0.5 * size.height()};
	return normalizedRgba(
	   m_fft(ButterworthFreq2d{size, m_orientation, freq_scaled}(m_fft(m_noise(size)))));
}
/*
Texpainter::Model::Image Texpainter::SurfaceGenerator::operator()(Tag<FilterGraph::Gaussian2d>,
Size2d)


Texpainter::Model::Image Texpainter::SurfaceGenerator::operator()(Tag<FilterGraph::Butt1d>, Size2d);
Texpainter::Model::Image Texpainter::SurfaceGenerator::operator()(Tag<FilterGraph::Butt1dXThenY>,
Size2d); Texpainter::Model::Image
Texpainter::SurfaceGenerator::operator()(Tag<FilterGraph::Butt1dXYSum>, Size2d);
*/