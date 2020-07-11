//@	{
//@	 "targets":[{"name":"color_picker.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"color_picker.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_COLORPICKER_HPP
#define TEXPAINTER_COLORPICKER_HPP

#include "./container.hpp"

#include "model/pixel.hpp"
#include "model/palette.hpp"
#include "utils/polymorphic_rng.hpp"

#include <utility>

namespace Texpainter::Ui
{
	class ColorPicker
	{
	public:
		explicit ColorPicker(Container& cnt,
		                     PolymorphicRng rng,
		                     char const* predef_label,
		                     std::span<Model::Pixel const> predef_colors);
		~ColorPicker();

		ColorPicker& operator=(ColorPicker&& obj) noexcept
		{
			std::swap(obj.m_impl, m_impl);
			return *this;
		}

		ColorPicker(ColorPicker&& obj) noexcept: m_impl(obj.m_impl)
		{
			obj.m_impl = nullptr;
		}

		Model::Pixel value() const noexcept;

		ColorPicker& value(Model::Pixel color);

		/** Returns a non-moving *this
		 */
		ColorPicker const& self() const noexcept;

		ColorPicker& self() noexcept
		{
			return const_cast<ColorPicker&>(std::as_const(*this).self());
		}

	private:
		class Impl;
		explicit ColorPicker(Impl& impl): m_impl{&impl}
		{
		}
		Impl* m_impl;
	};
}

#endif