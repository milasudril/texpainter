//@	{
//@	"targets":[{"name":"brushes.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_MODEL_BRUSHES_HPP
#define TEXPAINTER_MODEL_BRUSHES_HPP

#include "utils/vec_t.hpp"
#include "libenum/empty.hpp"

#include <cstdint>

namespace Texpainter::Model
{
	enum class BrushShape : uint32_t
	{
		Diamond,
		Circle,
		Square
	};

	constexpr BrushShape begin(Enum::Empty<BrushShape>) { return BrushShape::Diamond; }

	constexpr BrushShape end(Enum::Empty<BrushShape>)
	{
		return BrushShape{static_cast<uint32_t>(BrushShape::Square) + 1};
	}

	template<BrushShape>
	struct BrushTraits;

	template<>
	struct BrushTraits<BrushShape::Diamond>
	{
		static bool test(float radius, vec2_t loc)
		{
			auto loc_abs = loc < 0 ? -loc : loc;
			return loc_abs[0] + loc_abs[1] <= radius;
		}

		static constexpr char const* displayName() { return "⯁"; }

		static constexpr char const* name() { return "diamond"; }
	};

	template<>
	struct BrushTraits<BrushShape::Circle>
	{
		static bool test(float radius, vec2_t loc)
		{
			auto d2 = dot(loc, loc);
			return d2 <= radius * radius;
		}

		static constexpr char const* displayName() { return "⏺"; }

		static constexpr char const* name() { return "circle"; }
	};

	template<>
	struct BrushTraits<BrushShape::Square>
	{
		static bool test(float radius, vec2_t loc)
		{
			auto loc_abs = loc < 0 ? -loc : loc;
			auto loc_max = std::max(loc_abs[0], loc_abs[1]);
			return loc_max <= radius;
		}

		static constexpr char const* displayName() { return "⯀"; }

		static constexpr char const* name() { return "square"; }
	};
}
#endif
