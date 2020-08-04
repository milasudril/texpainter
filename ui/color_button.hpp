//@	{
//@  "targets":[{"name":"color_button.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"color_button.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_COLORBUTTON_HPP
#define TEXPAINTER_COLORBUTTON_HPP

#include "./container.hpp"
#include "./box.hpp"

#include "pixel_store/pixel.hpp"

#include <utility>

namespace Texpainter::Ui
{
	class ColorButton
	{
	public:
		explicit ColorButton(Container& cnt,
		                     PixelStore::Pixel color = PixelStore::Pixel{0.5, 0.5, 0.5, 1.0},
		                     double radius           = 0.5,
		                     vec2_t location         = vec2_t{0.0, 0.0});

		~ColorButton();

		ColorButton& operator=(ColorButton&& obj) noexcept
		{
			std::swap(obj.m_impl, m_impl);
			return *this;
		}

		ColorButton(ColorButton&& obj) noexcept: m_impl{obj.m_impl} { obj.m_impl = nullptr; }

		template<auto id, class EventHandler>
		ColorButton& eventHandler(EventHandler& eh);

	protected:
		class Impl;
		Impl* m_impl;
		explicit ColorButton(Impl& impl): m_impl(&impl) {}
	};
}

#endif
