//@	{
//@	"targets":[{"name":"brushes.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_MODEL_BRUSHES_HPP
#define TEXPAINTER_MODEL_BRUSHES_HPP

#include "utils/vec_t.hpp"
#include "utils/empty.hpp"

#include <cstdint>

namespace Texpainter::Model
{
	enum class BrushType : uint32_t
	{
		Diamond,
		Circle,
		Square
	};

	constexpr BrushType end(Empty<BrushType>)
	{
		return BrushType{static_cast<uint32_t>(BrushType::Square) + 1};
	}
	template<BrushType>
	struct BrushTraits;

	template<>
	struct BrushTraits<BrushType::Diamond>
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
	struct BrushTraits<BrushType::Circle>
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
	struct BrushTraits<BrushType::Square>
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
