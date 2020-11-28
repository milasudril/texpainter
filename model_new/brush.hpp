//@	{
//@	"targets":[{"name":"brush.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_MODEL_BRUSH_HPP
#define TEXPAINTER_MODEL_BRUSH_HPP

#include "./brushes.hpp"

#include "libenum/enum.hpp"
#include <array>

namespace Texpainter::Model
{
	class BrushInfo
	{
	public:
		explicit BrushInfo(float r, BrushShape t): m_radius{r}, m_type{t} {}

		BrushInfo& shape(BrushShape t)
		{
			m_type = t;
			return *this;
		}

		BrushShape shape() const { return m_type; }

		BrushInfo& radius(float r)
		{
			m_radius = r;
			return *this;
		}

		float radius() const { return m_radius; }

	private:
		float m_radius;
		BrushShape m_type;
	};

	namespace detail
	{
		using BrushFunc = bool (*)(float radius, vec2_t location);
		constexpr auto gen_brush_vtable()
		{
			std::array<BrushFunc, static_cast<size_t>(end(Enum::Empty<BrushShape>{}))> ret{};
			Enum::forEachEnumItem<BrushShape>([&ret](auto item) {
				ret[static_cast<int>(item.value)] = BrushTraits<item.value>::test;
			});
			return ret;
		}
		constexpr auto brush_vtable = gen_brush_vtable();
	}

	class BrushFunction
	{
	public:
		explicit BrushFunction(BrushShape type)
		    : r_func{detail::brush_vtable[static_cast<int>(type)]}
		{
		}

		decltype(auto) operator()(float radius, vec2_t location) const
		{
			return r_func(radius, location);
		}

	private:
		detail::BrushFunc r_func;
	};
}
#endif
