//@	{
//@	 "targets":[{"name":"menu.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"menu.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_UI_MENU_HPP
#define TEXPAINTER_UI_MENU_HPP

#include "./container.hpp"

#include <utility>

namespace Texpainter::Ui
{
	class Menu: public Container
	{
	public:
		explicit Menu(Container& parent);
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
		void* toplevel() const override;

	protected:
		class Impl;
		explicit Menu(Menu::Impl& impl): m_impl(&impl) {}
		Impl* m_impl;
	};
}

#endif
