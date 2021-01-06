//@	{
//@	"targets":[{"name":"topography_info.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_PIXELTYPES_TOPOGRAPHYINFO_HPP
#define TEXPAINTER_PIXELTYPES_TOPOGRAPHYINFO_HPP

#include "utils/vec_t.hpp"

namespace Texpainter::Model
{
	class TopographyInfo
	{
	public:
		TopographyInfo(): m_data{0.0f, 0.0f, 1.0f, 0.0f} {}

		explicit TopographyInfo(float d_x, float d_y, float elevation)
		{
			vec4_t normal{-d_x, -d_y, 1.0f, 0.0f};
			normal /= length(normal);
			m_data = vec4_t{normal[0], normal[1], normal[2], elevation};
		}

		vec4_t value() const { return m_data; }

		vec4_t normal() const { return vec4_t{m_data[0], m_data[1], m_data[2], 0.0f}; }

		float elevation() const { return m_data[3]; }

	private:
		vec4_t m_data;
	};

}

#endif