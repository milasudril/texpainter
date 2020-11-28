//@	{
//@	"targets":[{"name":"brushes.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_MODEL_BRUSHES_HPP
#define TEXPAINTER_MODEL_BRUSHES_HPP

#include "utils/vec_t.hpp"
#include "utils/empty.hpp"

#include "libenum/enum.hpp"

#include <cstdint>

namespace Texpainter::Model
{
	enum class BrushType : uint32_t
	{
		Diamond,
		Circle,
		Square
	};

	constexpr BrushType end(Enum::Empty<BrushType>)
	{
		return BrushType{static_cast<uint32_t>(BrushType::Square) + 1};
	}

	constexpr BrushType begin(Enum::Empty<BrushType>) { return BrushType::Diamond; }

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

		static constexpr char const* displayName() { return "⯁ Diamond"; }
	};

	template<>
	struct BrushTraits<BrushType::Circle>
	{
		static bool test(float radius, vec2_t loc)
		{
			auto d2 = dot(loc, loc);
			return d2 <= radius * radius;
		}

		static constexpr char const* displayName() { return "⏺ Circle"; }
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

		static constexpr char const* displayName() { return "⯀ Square"; }
	};
}
#endif
