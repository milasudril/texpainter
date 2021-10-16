//@	{
//@	 "targets":[{"name":"topography_info.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"topography_info.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_PIXELSTORE_TOPOGRAPHYINFO_HPP
#define TEXPAINTER_PIXELSTORE_TOPOGRAPHYINFO_HPP

#include "utils/vec_t.hpp"
#include "utils/size_2d.hpp"

#include <memory>

namespace Texpainter::PixelStore
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

		explicit TopographyInfo(vec4_t data)
		{
			auto normal = vec4_t{data[0], data[1], data[2], 0.0f};
			normal /= length(normal);
			auto elevation = data[3];
			m_data         = vec4_t{normal[0], normal[1], normal[2], elevation};
		}

		vec4_t value() const { return m_data; }

		vec4_t normal() const { return vec4_t{m_data[0], m_data[1], m_data[2], 0.0f}; }

		float elevation() const { return m_data[3]; }

	private:
		vec4_t m_data;
	};

	std::unique_ptr<TopographyInfo[]> downsample(Texpainter::Size2d size,
	                                             TopographyInfo const* src,
	                                             uint32_t scale);
}

#endif