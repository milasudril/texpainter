//@	{
//@	"targets":[{"name":"blend_function.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_MODEL_BLENDFUNCTION_HPP
#define TEXPAINTER_MODEL_BLENDFUNCTION_HPP

#include "model/blend_modes.hpp"

namespace Texpainter::Model
{
	class BlendFunctionInfo
	{
	public:
		BlendFunctionInfo(): BlendFunctionInfo(1.0f, BlendMode::SourceOver) {}

		explicit BlendFunctionInfo(float strength, BlendMode mode)
		    : m_strength{strength}
		    , m_mode{mode}
		{
		}

		BlendFunctionInfo& mode(BlendMode mode)
		{
			m_mode = mode;
			return *this;
		}

		BlendMode mode() const { return m_mode; }

		BlendFunctionInfo& radius(float strength)
		{
			m_strength = strength;
			return *this;
		}

		float strength() const { return m_strength; }

	private:
		float m_strength;
		BlendMode m_mode;
	};

	namespace detail
	{
		using BlendFunctionFunc = PixelStore::Pixel (*)(PixelStore::Pixel dest,
		                                                PixelStore::Pixel source);
		constexpr auto gen_blendfunc_vtable()
		{
			std::array<BlendFunctionFunc, static_cast<size_t>(end(Empty<BlendMode>{}))> ret{};
			forEachEnumItem<BlendMode>([&ret](auto item) {
				ret[static_cast<int>(item.value)] = BlendModeTraits<item.value>::blend;
			});
			return ret;
		}
		constexpr auto blendfunc_vtable = gen_blendfunc_vtable();
	}

	class BlendFunction
	{
	public:
		BlendFunction(): BlendFunction(BlendMode::SourceOver) {}

		explicit BlendFunction(BlendMode mode)
		    : r_func{detail::blendfunc_vtable[static_cast<int>(mode)]}
		    , m_id{mode}
		{
		}

		decltype(auto) operator()(PixelStore::Pixel dest,
		                          PixelStore::Pixel source,
		                          float strength) const
		{
			auto ret = r_func(dest, source);
			return strength * ret + (1.0f - strength) * dest;
		}

		BlendMode id() const { return m_id; }

	private:
		detail::BlendFunctionFunc r_func;
		BlendMode m_id;
	};
}
#endif
