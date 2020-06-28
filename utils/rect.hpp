//@	{"targets":[{"name":"rect.hpp", "type":"include"}]}

#ifndef TEXPAINTER_UTILS_RECT_HPP
#define TEXPAINTER_UTILS_RECT_HPP

#include "./vec_t.hpp"
#include "./angle.hpp"

namespace Texpainter::Utils
{
	vec2_t axisAlignedBoundingBox(vec2_t size, Angle ϴ)
	{
		auto const corner = size;
		auto const rot_x = vec2_t{cos(ϴ), -sin(ϴ)};
		auto const rot_y = vec2_t{sin(ϴ), cos(ϴ)};
		auto const corner_a_rot = transform(corner, rot_x, rot_y);
		// Mirror in x axis (wroks due to symmetry)
		auto const corner_b_rot = transform(corner * vec2_t{1.0, -1.0}, rot_x, rot_y);

		return vec2_t{std::max(std::abs(corner_a_rot[0]), std::abs(corner_b_rot[0])),
		              std::max(std::abs(corner_a_rot[1]), std::abs(corner_b_rot[1]))};
	}
}

#endif