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
#include <stack>

namespace Texpainter::Generators
{
	namespace detail
	{
		template<class Rng>
		void draw_line(Span2d<Model::Pixel> img, vec2_t from, vec2_t to, double line_width, Rng& rng)
		{
			auto const dir = to - from;
			auto const l = length(dir);
			auto const v = dir/l;
			auto const n = vec2_t{-v[1], v[0]};
			std::uniform_real_distribution<float> max_depth;
			for(int t = 0; t < static_cast<int>(l + 0.5); ++t)
			{
				for(int s = -static_cast<int>(line_width); s < static_cast<int>(line_width + 0.5); ++s)
				{
					auto const pos = from + static_cast<double>(t)*v + static_cast<double>(s)*0.5*n;
					auto pos_x = static_cast<int>(pos[0]);
					auto pos_y = static_cast<int>(pos[1]);
					pos_x = (pos_x < 0 ? pos_x + img.width() : pos_x)%img.width();
					pos_y = (pos_y < 0 ? pos_y + img.height() : pos_y)%img.height();
					auto val = max_depth(rng);
					img(pos_x, pos_y) = Model::Pixel{val, val, val, 1.0};
				}
			}
		}

		struct LineSeg
		{
			vec2_t x;
			Angle dir;
			float width;
			int level;
		};
	}

	class CrackGenerator
	{
	public:
		using Rng = std::mt19937;

		CrackGenerator():
		   m_n_cracks{2},
		   m_line_width{1.0f/128.0f},
		   m_seg_length{1.0/64.0},
		   m_branch_prob{0.25},
		   m_max_length{0.25}
		{
		}

		Model::Image operator()(Size2d output_size)
		{
			auto size = sqrt(output_size.area());
			std::uniform_int_distribution<uint32_t> init_pos_x{0, output_size.width() - 1};
			std::uniform_int_distribution<uint32_t> init_pos_y{0, output_size.height() - 1};
			std::exponential_distribution<double> seg_length_dist{1.0/(m_seg_length * size)};

			std::uniform_int_distribution<int32_t> dir_dist{
				static_cast<int32_t>(Angle{-0.1666, Angle::Turns{}}.bits()),
				static_cast<int32_t>(Angle{0.1666, Angle::Turns{}}.bits())};

			std::uniform_int_distribution<int32_t> dir_dist_split_a{
				static_cast<int32_t>(Angle{-0.25, Angle::Turns{}}.bits()),
				static_cast<int32_t>(Angle{-0.08333, Angle::Turns{}}.bits())};

			std::uniform_int_distribution<int32_t> dir_dist_split_b{
				static_cast<int32_t>(Angle{0.08333, Angle::Turns{}}.bits()),
				static_cast<int32_t>(Angle{0.25, Angle::Turns{}}.bits())};

			auto const branch_prob = m_branch_prob;
			std::bernoulli_distribution branch{branch_prob};
			Model::Image ret{output_size};
			std::ranges::fill(ret.pixels(), Model::Pixel{0.0, 0.0, 0.0, 1.0f});
			auto const line_width = static_cast<float>(m_line_width*size);
			auto const max_length = m_max_length;
			auto n = m_n_cracks;
			while(n != 0)
			{
				std::stack<detail::LineSeg> segs;
				segs.push(detail::LineSeg{
					vec2_t{static_cast<double>(init_pos_x(m_rng)), static_cast<double>(init_pos_y(m_rng))},
					Angle{static_cast<uint32_t>(dir_dist(m_rng))},
					line_width,
					1});
				while(!segs.empty())
				{
					auto line = segs.top();
					segs.pop();
					auto traveled_distance = 0.0;
					while(traveled_distance < max_length*size/line.level)
					{
						auto const seg_length = seg_length_dist(m_rng);
						if(seg_length > size/64.0)
						{
							auto x_next = line.x + seg_length*vec2_t{cos(line.dir), sin(line.dir)};
							traveled_distance += seg_length;
							detail::draw_line(ret.pixels(), line.x, x_next, line.width, m_rng);
							line.width *= 0.95;
							if(branch(m_rng))
							{
								line.x = x_next;
								line.dir += Angle{static_cast<uint32_t>(dir_dist_split_a(m_rng))};
								segs.push(detail::LineSeg{x_next, Angle{static_cast<uint32_t>(dir_dist_split_b(m_rng))}, line.width,
								line.level + 1});
							}
							else
							{
								line.dir += Angle{static_cast<uint32_t>(dir_dist(m_rng))};
								line.x = x_next;
							}
						}
					}
				}
				--n;
			}
			return ret;
		}

	private:
		Rng m_rng;
		int m_n_cracks;
		float m_line_width;
		double m_seg_length;
		double m_branch_prob;
		double m_max_length;

	};
}

#endif