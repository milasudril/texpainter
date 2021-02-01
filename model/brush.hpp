//@	{
//@	"targets":[{"name":"brush.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_MODEL_BRUSH_HPP
#define TEXPAINTER_MODEL_BRUSH_HPP

#include "./brushes.hpp"

#include "libenum/enum.hpp"

#define JSON_USE_IMPLICIT_CONVERSIONS 0
#include <nlohmann/json.hpp>

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
		using BrushFunc = bool (*)(vec2_t location, double radius);
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

		decltype(auto) operator()(vec2_t location, double radius) const
		{
			return r_func(location, radius);
		}

	private:
		detail::BrushFunc r_func;
	};
}


namespace nlohmann
{
    template <>
    struct adl_serializer<Texpainter::Model::BrushInfo>
    {
        static Texpainter::Model::BrushInfo from_json(const json& j)
		{
			auto radius = j.at("radius").get<float>();
			auto shape = j.at("shape").get<Texpainter::Model::BrushShape>();
            return Texpainter::Model::BrushInfo{radius, shape};
        }

        static void to_json(json& j, Texpainter::Model::BrushInfo brush)
		{
			j["radius"] = brush.radius();
			j["shape"]  = brush.shape();
		}
    };
}
#endif
