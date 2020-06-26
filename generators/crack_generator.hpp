//@	{
//@	  "targets":[{"name":"crack_generator.hpp", "type":"include"}]
//@	 }

#ifndef TEXPAINTER_CRACKGENERATOR_HPP
#define TEXPAINTER_CRACKGENERATOR_HPP

#include "model/image.hpp"
#include "utils/angle.hpp"

#include <random>
#include <limits>
#include <ranges>

namespace Texpainter::Generators
{
	namespace detail
	{
		template<class Rng, class Distribution>
		void draw_line(Span2d<Model::Pixel> img, vec2_t from, vec2_t to, double, Rng&, Distribution&)
		{
			auto const dir = to - from;
			auto const l = length(dir);
			auto const v = dir/l;
		//	TODO: auto const n = vec2_t{-v[1], v[0]};
			for(int t = 0; t < static_cast<int>(l + 0.5); ++t)
			{
				auto const pos = from + static_cast<double>(t)*v;
				auto pos_x = static_cast<int>(pos[0]);
				auto pos_y = static_cast<int>(pos[1]);
				pos_x = (pos_x < 0 ? pos_x + img.width() : pos_x)%img.width();
				pos_y = (pos_y < 0 ? pos_y + img.height() : pos_y)%img.height();
				img(pos_x, pos_y) = Model::Pixel{1.0, 1.0, 1.0, 1.0};
			}
		}
	}

	class CrackGenerator
	{
	public:
		using Rng = std::mt19937;

		CrackGenerator():
		   m_intensity{1.0/16},
		   m_impulse_size{1.0/32.0},
		   m_line_width{1.0/16.0}
		{
		}

		Model::Image operator()(Size2d output_size)
		{
			auto size = sqrt(output_size.area());
			auto n = static_cast<int>(m_intensity*sqrt(output_size.area()));
			std::uniform_int_distribution<uint32_t> init_pos_x{0, output_size.width() - 1};
			std::uniform_int_distribution<uint32_t> init_pos_y{0, output_size.height() - 1};
			std::exponential_distribution<double> impulse_size{1.0/(m_impulse_size * size)};
			std::uniform_int_distribution<uint32_t> impulse_direction{0, std::numeric_limits<uint32_t>::max()};
			std::uniform_real_distribution<double> max_depth;
			Model::Image ret{output_size};
			std::ranges::fill(ret.pixels(), Model::Pixel{0.0, 0.0, 0.0, 1.0f});
			constexpr auto dt = 1.0;
			while(n != 0)
			{
				vec2_t x{static_cast<double>(init_pos_x(m_rng)), static_cast<double>(init_pos_y(m_rng))};
				auto line_width = m_line_width*size;
				while(static_cast<int>(line_width) != 0)
				{
					auto impulse = impulse_size(m_rng);
					auto dir = Angle{impulse_direction(m_rng)};
					auto x_next = x +  impulse*vec2_t{cos(dir), sin(dir)} * dt;
					detail::draw_line(ret.pixels(), x, x_next, line_width, m_rng, max_depth);
					line_width *= 0.75;
					x = x_next;
				}
				--n;
			}
			return ret;
		}

	private:
		Rng m_rng;
		double m_intensity;
		double m_impulse_size;
		double m_line_width;

	};
}

#endif