//@	{
//@	 "targets":[{"name":"menu_bar.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"menu_bar.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_UI_MENUBAR_HPP
#define TEXPAINTER_UI_MENUBAR_HPP

#include "./container.hpp"

#include <utility>

namespace Texpainter::Ui
{
	class MenuBar: public Container
	{
	public:
		explicit MenuBar(Container& parent);
		~MenuBar();

		MenuBar& operator=(MenuBar&& obj) noexcept
		{
			std::swap(obj.m_impl, m_impl);
			return *this;
		}

		MenuBar(MenuBar&& obj) noexcept: m_impl(obj.m_impl) { obj.m_impl = nullptr; }

		MenuBar& add(void* handle) override;
		MenuBar& show() override;
		MenuBar& sensitive(bool val) override;
		void* toplevel() const override;

	protected:
		class Impl;
		explicit MenuBar(MenuBar::Impl& impl): m_impl(&impl) {}
		Impl* m_impl;
	};
}

#endif
