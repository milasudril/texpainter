//@	{
//@	  "targets":[{"name":"crack_generator.o", "type":"object"}]
//@	 }

#include "./crack_generator.hpp"

#include "model/image.hpp"
#include "utils/angle.hpp"

#include <random>
#include <limits>
#include <ranges>
#include <stack>

namespace
{
	template<class Rng>
	void draw_line(Texpainter::Span2d<Texpainter::Model::Pixel> img,
	               Texpainter::vec2_t from,
	               Texpainter::vec2_t to,
	               double line_width,
	               Rng& rng,
	               int level)
	{
		auto const dir = to - from;
		auto const l = Texpainter::length(dir);
		auto const t = dir / l;
		auto const n = Texpainter::vec2_t{-t[1], t[0]};
		std::uniform_real_distribution<double> max_depth;
		for(int t_i = 0; t_i < static_cast<int>(l + 0.5); ++t_i)
		{
			for(int n_i = -static_cast<int>(line_width); n_i < static_cast<int>(line_width + 0.5); ++n_i)
			{
				auto const pos = from + static_cast<double>(t_i) * t + static_cast<double>(n_i) * 0.5 * n;
				auto pos_x = static_cast<int>(pos[0]);
				auto pos_y = static_cast<int>(pos[1]);
				pos_x = (pos_x < 0 ? pos_x + img.width() : pos_x) % img.width();
				pos_y = (pos_y < 0 ? pos_y + img.height() : pos_y) % img.height();
				auto const n_pos = static_cast<double>(n_i) / line_width;
				auto val = static_cast<float>(1.0 - n_pos * n_pos * (0.5 * max_depth(rng) + 0.5));
				img(pos_x, pos_y) = Texpainter::Model::Pixel{val, val, val, 1.0};
			}
		}
	}

	struct LineSeg
	{
		Texpainter::vec2_t x;
		Texpainter::Angle dir;
		float width;
		int level;
	};
}

Texpainter::Model::Image Texpainter::Generators::CrackGenerator::operator()(Size2d output_size)
{
	auto size = sqrt(output_size.area());
	std::uniform_int_distribution<uint32_t> init_pos_x{0, output_size.width() - 1};
	std::uniform_int_distribution<uint32_t> init_pos_y{0, output_size.height() - 1};
	std::exponential_distribution<double> seg_length_dist{1.0 / (m_seg_length * size)};

	std::uniform_int_distribution<uint32_t> dir_dist_init{0, std::numeric_limits<uint32_t>::max()};

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
	auto const line_width = static_cast<float>(m_line_width * size);
	auto const max_length = m_max_length;
	auto n = m_n_cracks;
	while(n != 0)
	{
		std::stack<LineSeg> segs;

		segs.push(
		LineSeg{vec2_t{static_cast<double>(init_pos_x(r_rng)), static_cast<double>(init_pos_y(r_rng))},
				Angle{static_cast<uint32_t>(dir_dist_init(r_rng))},
				line_width,
				1});

		while(!segs.empty())
		{
			auto line = segs.top();
			segs.pop();
			auto traveled_distance = 0.0;
			while(traveled_distance < max_length * size / line.level)
			{
				auto const seg_length = seg_length_dist(r_rng);
				if(seg_length > size / 64.0)
				{
					auto x_next = line.x + seg_length * Texpainter::vec2_t{cos(line.dir), sin(line.dir)};
					traveled_distance += seg_length;
					draw_line(ret.pixels(), line.x, x_next, line.width, r_rng, line.level);
					line.width *= 0.75;
					if(branch(r_rng))
					{
						segs.push(LineSeg{x_next,
						                  line.dir + Angle{static_cast<uint32_t>(dir_dist_split_b(r_rng))},
						                  line.width,
						                  line.level + 1});
						segs.push(LineSeg{x_next,
						                  line.dir + Angle{static_cast<uint32_t>(dir_dist_split_a(r_rng))},
						                  line.width,
						                  line.level + 1});
						break;
					}
					else
					{
						line.dir += Angle{static_cast<uint32_t>(dir_dist(r_rng))};
						line.x = x_next;
					}
				}
			}
		}
		--n;
	}
	return ret;
}