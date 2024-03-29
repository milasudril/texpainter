//@	{
//@	 "targets":[{"name":"color_picker.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"color_picker.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_UI_COLORPICKER_HPP
#define TEXPAINTER_UI_COLORPICKER_HPP

#include "./container.hpp"

#include "pixel_store/rgba_value.hpp"
#include "pixel_store/palette.hpp"
#include "utils/polymorphic_rng.hpp"

#include <utility>
#include <span>

namespace Texpainter::Ui
{
	class ColorPicker
	{
	public:
		explicit ColorPicker(Container& cnt,
		                     PolymorphicRng rng,
		                     char const* predef_label,
		                     std::span<PixelStore::RgbaValue const> predef_colors);
		~ColorPicker();

		ColorPicker& operator=(ColorPicker&& obj) noexcept
		{
			std::swap(obj.m_impl, m_impl);
			return *this;
		}

		ColorPicker(ColorPicker&& obj) noexcept: m_impl(obj.m_impl) { obj.m_impl = nullptr; }

		PixelStore::RgbaValue value() const noexcept;

		ColorPicker& value(PixelStore::RgbaValue color);

		/** Returns a non-moving *this
		 */
		ColorPicker const& self() const noexcept;

		ColorPicker& self() noexcept
		{
			return const_cast<ColorPicker&>(std::as_const(*this).self());
		}

	private:
		class Impl;
		explicit ColorPicker(Impl& impl): m_impl{&impl} {}
		Impl* m_impl;
	};
}

#endif