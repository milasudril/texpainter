//@	{
//@	"targets":[{"name":"blend_modes.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_MODEL_BLENDMODES_HPP
#define TEXPAINTER_MODEL_BLENDMODES_HPP

#include "utils/empty.hpp"
#include "pixel_store/pixel.hpp"

#include <cstdint>

namespace Texpainter::Model
{
	enum class BlendMode : uint32_t
	{
		SourceOver,
		Multiply,
		AbsDifference
	};

	constexpr BlendMode end(Empty<BlendMode>)
	{
		return BlendMode{static_cast<uint32_t>(BlendMode::AbsDifference) + 1};
	}
	template<BlendMode>
	struct BlendModeTraits;

	template<>
	struct BlendModeTraits<BlendMode::SourceOver>
	{
		static constexpr PixelStore::Pixel blend(PixelStore::Pixel dest, PixelStore::Pixel source)
		{
			return source + (1.0f - source.alpha()) * dest;
		}

		static constexpr char const* displayName() { return "Source over"; }
	};

	template<>
	struct BlendModeTraits<BlendMode::Multiply>
	{
		static constexpr PixelStore::Pixel blend(PixelStore::Pixel dest, PixelStore::Pixel source)
		{
			return source * dest;
		}

		static constexpr char const* displayName() { return "Multiply"; }
	};

	template<>
	struct BlendModeTraits<BlendMode::AbsDifference>
	{
		static constexpr PixelStore::Pixel blend(PixelStore::Pixel dest, PixelStore::Pixel source)
		{
			return absDiff(dest, source);
		}

		static constexpr char const* displayName() { return "Absolute difference"; }
	};

}
#endif
