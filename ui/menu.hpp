//@	{
//@	 "targets":[{"name":"menu.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"menu.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_UI_MENU_HPP
#define TEXPAINTER_UI_MENU_HPP

#include "./container.hpp"
#include "menu_item.hpp"

#include <utility>

namespace Texpainter::Ui
{
	class Menu: public Container
	{
	public:
		Menu(): Menu{nullptr} {}

		explicit Menu(MenuItem& parent): Menu{static_cast<Container*>(&parent)} {}
		~Menu();

		Menu& operator=(Menu&& obj) noexcept
		{
			std::swap(obj.m_impl, m_impl);
			return *this;
		}

		Menu(Menu&& obj) noexcept: m_impl(obj.m_impl) { obj.m_impl = nullptr; }

		Menu& add(void* handle) override;
		Menu& show() override;
		Menu& sensitive(bool val) override;
		Menu& killFocus() override { return *this; }

		void* toplevel() const override;

		Menu& popupAtCursor();

	protected:
		class Impl;
		explicit Menu(Menu::Impl& impl): m_impl(&impl) {}
		Impl* m_impl;

	private:
		explicit Menu(Container* parent);
	};
}

#endif
