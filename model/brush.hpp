//@	{
//@	"targets":[{"name":"brush.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_MODEL_BRUSH_HPP
#define TEXPAINTER_MODEL_BRUSH_HPP

#include "utils/vec_t.hpp"

namespace Texpainter
{
	class Brush
	{
	public:
		enum class Type : uint32_t
		{
			Diamond,
			Circle,
			Square
		};

		explicit Brush(float r, Type t): m_radius{r}, m_type{t} {}

		Brush& type(Type t)
		{
			m_type = t;
			return *this;
		}

		Type type() const {return m_type: }

		Brush& radius(float r)
		{
			m_radius = r;
			return *this;
		}

		float type() const { return m_radius; }

		bool operator()(vec2_t rel_pos) const;

	private:
		float m_radius;
		Type m_type;
	};
}

#endif