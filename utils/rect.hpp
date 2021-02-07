//@	{"targets":[{"name":"rect.hpp", "type":"include"}]}

#ifndef TEXPAINTER_UTILS_RECT_HPP
#define TEXPAINTER_UTILS_RECT_HPP

#include "./vec_t.hpp"
#include "./angle.hpp"

namespace Texpainter
{
	class AxisAlignedBoundingBox
	{
	public:
		explicit AxisAlignedBoundingBox(vec2_t origin, vec2_t radius):
		m_origin{origin}, m_radius{radius}
		{
		}

		static AxisAlignedBoundingBox fromLimits(vec2_t lower, vec2_t upper)
		{
			return AxisAlignedBoundingBox{0.5*(upper + lower), 0.5*(upper - lower)};
		}

		vec2_t origin() const { return m_origin; }
		vec2_t radius() const { return m_radius; }

	private:
		vec2_t m_origin;
		vec2_t m_radius;
	};

	inline auto upperLimit(AxisAlignedBoundingBox box)
	{
		return box.origin() + box.radius();
	}

	inline auto lowerLimit(AxisAlignedBoundingBox box)
	{
		return box.origin() - box.radius();
	}

	inline auto size(AxisAlignedBoundingBox box)
	{
		return 2.0*box.radius();
	}

	inline AxisAlignedBoundingBox rotate(AxisAlignedBoundingBox box, Angle ϴ)
	{
		auto const rot_x = vec2_t{cos(ϴ), -sin(ϴ)};
		auto const rot_y = vec2_t{sin(ϴ), cos(ϴ)};
		auto const corner_a = abs(transform(box.radius(), rot_x, rot_y));
		auto const corner_b = abs(transform(box.radius()*vec2_t{1.0, -1.0}, rot_x, rot_y));
		auto const radius = max(corner_a, corner_b);
		return AxisAlignedBoundingBox{box.origin(), radius};
	}

	inline vec2_t axisAlignedBoundingBox(vec2_t size, Angle ϴ)
	{
		return rotate(AxisAlignedBoundingBox{vec2_t{0.0, 0.0}, size}, ϴ).radius();
#if 0
		auto const corner       = size;
		auto const rot_x        = vec2_t{cos(ϴ), -sin(ϴ)};
		auto const rot_y        = vec2_t{sin(ϴ), cos(ϴ)};
		auto const corner_a_rot = transform(corner, rot_x, rot_y);
		// Mirror in x axis (wroks due to symmetry)
		auto const corner_b_rot = transform(corner * vec2_t{1.0, -1.0}, rot_x, rot_y);

		return vec2_t{std::max(std::abs(corner_a_rot[0]), std::abs(corner_b_rot[0])),
		              std::max(std::abs(corner_a_rot[1]), std::abs(corner_b_rot[1]))};
#endif
	}

	inline vec2_t axisAlignedBoundingBox(vec2_t size)
	{
		auto const corner_a = size;
		auto const corner_b = corner_a * vec2_t{1.0, -1.0};

		return vec2_t{std::max(std::abs(corner_a[0]), std::abs(corner_b[0])),
		              std::max(std::abs(corner_a[1]), std::abs(corner_b[1]))};
	}


}

#endif