//@	{
//@	"targets":[{"name":"brushes.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_MODEL_BRUSHES_HPP
#define TEXPAINTER_MODEL_BRUSHES_HPP

#include "utils/vec_t.hpp"
#include "utils/empty.hpp"

#include <cstdint>

#include <cstdio>

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
		static bool test(float radius, vec2_t pos)
		{
			auto pos_abs = pos < 0 ? -pos : pos;
			return pos_abs[0] + pos_abs[1] <= radius;
		}

		static constexpr char const* displayName() { return "⯁ Diamond"; }
	};

	template<>
	struct BrushTraits<BrushType::Circle>
	{
		static bool test(float radius, vec2_t pos)
		{
			auto d2 = dot(pos, pos);
			return d2 <= radius * radius;
		}

		static constexpr char const* displayName() { return "⏺ Circle"; }
	};

	template<>
	struct BrushTraits<BrushType::Square>
	{
		static bool test(float radius, vec2_t pos)
		{
			auto pos_abs = pos < 0 ? -pos : pos;
			auto pos_max = std::max(pos_abs[0], pos_abs[1]);
			return pos_max <= radius;
		}

		static constexpr char const* displayName() { return "⯀ Square"; }
	};
}
#endif
