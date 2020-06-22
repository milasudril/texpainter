//@	{
//@	  "targets":[{"name":"surface_generator.o", "type":"object"}]
//@	 }

#include "./surface_generator.hpp"
#include "./butterworth_freq_2d.hpp"
#include "./gaussian_freq_2d.hpp"
#include "./butterworth_freq_1d.hpp"

namespace
{
	Texpainter::Model::Image normalizedRgba(Texpainter::Span2d<double const> source)
	{
		auto res = std::ranges::minmax_element(source);
		Texpainter::Model::Image ret{source.size()};
		std::ranges::transform(source, ret.pixels().begin(), [min = *res.min, max = *res.max](auto val) {
			Texpainter::vec4_double_t const offset{-min, -min, -min, 0.0};
			Texpainter::vec4_double_t const factor{max - min, max - min, max - min, 1.0};

			auto const output = (Texpainter::vec4_double_t{val, val, val, 1.0} + offset) / factor;

			return Texpainter::Model::Pixel{static_cast<float>(output[0]),
			                                static_cast<float>(output[1]),
			                                static_cast<float>(output[2]),
			                                static_cast<float>(output[3])};
		});
		return ret;
	}
}

Texpainter::Model::Image Texpainter::Generators::SurfaceGenerator::
operator()(Tag<FilterGraph::None>, Size2d size)
{
	return normalizedRgba(m_noise(size));
}

Texpainter::Model::Image Texpainter::Generators::SurfaceGenerator::
operator()(Tag<FilterGraph::Butt2d>, Size2d size)
{
	auto freq_scaled = m_cutoff_freq * vec2_t{0.5 * size.width(), 0.5 * size.height()};
	return normalizedRgba(
	   m_fft(ButterworthFreq2d{size, m_orientation, freq_scaled}(m_fft(m_noise(size)))));
}

Texpainter::Model::Image Texpainter::Generators::SurfaceGenerator::
operator()(Tag<FilterGraph::Gaussian2d>, Size2d size)
{
	auto freq_scaled = m_cutoff_freq * vec2_t{0.5 * size.width(), 0.5 * size.height()};
	return normalizedRgba(
	   m_fft(GaussianFreq2d{size, m_orientation, freq_scaled}(m_fft(m_noise(size)))));
}

Texpainter::Model::Image Texpainter::Generators::SurfaceGenerator::
operator()(Tag<FilterGraph::Butt1d>, Size2d size)
{
	auto freq_scaled = m_cutoff_freq * vec2_t{0.5 * size.width(), 0.5 * size.height()};
	return normalizedRgba(m_fft(ButterworthFreq1d{
	   size, m_orientation, mean(freq_scaled.ξ(), freq_scaled.η())}(m_fft(m_noise(size)))));
}

Texpainter::Model::Image Texpainter::Generators::SurfaceGenerator::
operator()(Tag<FilterGraph::Butt1dXThenY>, Size2d size)
{
	auto freq_scaled = m_cutoff_freq * vec2_t{0.5 * size.width(), 0.5 * size.height()};

	ButterworthFreq1d filter_x{size, m_orientation, freq_scaled.ξ()};
	ButterworthFreq1d filter_y{size, m_orientation + Angle{0.25, Angle::Turns{}}, freq_scaled.η()};

	return normalizedRgba(m_fft(filter_y(filter_x(m_fft(m_noise(size))))));
}

Texpainter::Model::Image Texpainter::Generators::SurfaceGenerator::
operator()(Tag<FilterGraph::Butt1dXYSum>, Size2d size)
{
	auto freq_scaled = m_cutoff_freq * vec2_t{0.5 * size.width(), 0.5 * size.height()};

	ButterworthFreq1d filter_x{size, m_orientation, freq_scaled.ξ()};
	ButterworthFreq1d filter_y{size, m_orientation + Angle{0.25, Angle::Turns{}}, freq_scaled.η()};

	auto input = m_fft(m_noise(size));
	auto output_a = filter_x(input);
	auto output_b = filter_y(input);

	std::ranges::transform(output_a.pixels(),
	                       output_b.pixels(),
	                       output_b.pixels().begin(),
	                       [](auto a, auto b) { return 0.5 * (a + b); });

	return normalizedRgba(m_fft(output_b));
}