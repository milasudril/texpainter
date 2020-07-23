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

	static_assert(static_cast<int>(end(Empty<BrushType>{})) == 3, "???");

	template<BrushType>
	struct BrushTraits;

	template<>
	struct BrushTraits<BrushType::Diamond>
	{
		static float intensity(float radius, vec2_t pos)
		{
			auto pos_abs = pos < 0 ? -pos : pos;
			return static_cast<float>(pos_abs[0] + pos_abs[1] <= radius);
		}

		static constexpr char const* displayName() { return "⯁ Diamond"; }
	};

	template<>
	struct BrushTraits<BrushType::Circle>
	{
		static float intensity(float radius, vec2_t pos)
		{
			auto d2 = dot(pos, pos);
			return static_cast<float>(d2 <= radius * radius);
		}

		static constexpr char const* displayName() { return "⏺ Circle"; }
	};

	template<>
	struct BrushTraits<BrushType::Square>
	{
		static float intensity(float radius, vec2_t pos)
		{
			auto pos_abs = pos < 0 ? -pos : pos;
			auto pos_max = std::max(pos_abs[0], pos_abs[1]);
			return static_cast<float>(pos_max <= radius);
		}

		static constexpr char const* displayName() { return "⯀ Square"; }
	};
}
#endif
