//@	{
//@	"targets":[{"name":"brush.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_MODEL_BRUSH_HPP
#define TEXPAINTER_MODEL_BRUSH_HPP

#include "./brushes.hpp"

#include "utils/empty.hpp"

namespace Texpainter::Model
{
	class BrushInfo
	{
	public:
		explicit BrushInfo(float r, BrushType t): m_radius{r}, m_type{t} {}

		BrushInfo& type(BrushType t)
		{
			m_type = t;
			return *this;
		}

		BrushType type() const { return m_type; }

		BrushInfo& radius(float r)
		{
			m_radius = r;
			return *this;
		}

		float radius() const { return m_radius; }

	private:
		float m_radius;
		BrushType m_type;
	};

	namespace detail
	{
		using BrushFunc = float (*)(float radius, vec2_t position);
		constexpr auto gen_brush_vtable()
		{
			std::array<BrushFunc, static_cast<size_t>(end(Empty<BrushType>{}))> ret{};
			forEachEnumItem<BrushType>([&ret](auto item) {
				ret[static_cast<int>(item.value)] = BrushTraits<item.value>::intensity;
			});
			return ret;
		}
		constexpr auto brush_vtable = gen_brush_vtable();
	}

	class BrushFunction
	{
	public:
		explicit BrushFunction(BrushType type): r_func{detail::brush_vtable[static_cast<int>(type)]}
		{
		}

		decltype(auto) operator()(float radius, vec2_t position) const
		{
			return r_func(radius, position);
		}

	private:
		detail::BrushFunc r_func;
	};
}
#endif
