//@	{
//@	 "targets":[{"name":"window.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"window.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_UI_WINDOW_HPP
#define TEXPAINTER_UI_WINDOW_HPP

#include "./container.hpp"

#include "geom/dimension.hpp"

#include <utility>

namespace Texpainter::Ui
{
	class Window: public Container
	{
	public:
		explicit Window(const char* title, Container* owner = nullptr);
		~Window() override;

		Window& operator=(Window&& obj) noexcept
		{
			std::swap(obj.m_impl, m_impl);
			return *this;
		}

		Window(Window&& obj) noexcept: m_impl(obj.m_impl)
		{
			obj.m_impl = nullptr;
		}

		Window& add(void* handle) override;
		Window& show() override;
		Window& sensitive(bool val) override;
		void* toplevel() const override;

		const char* title() const noexcept;
		Window& title(const char* title_new);

		template<auto id, class WindowCallback>
		Window& callback(WindowCallback& cb) noexcept
		{
			auto cb_wrapper = [](void* rvc, Window& self) {
				auto x = reinterpret_cast<WindowCallback*>(rvc);
				x->template closing<id>(self);
			};
			return callback(cb_wrapper, &cb);
		}

		Window& modal(bool state);

		Window& defaultSize(Geom::Dimension dim);

		static void terminateApp();

	protected:
		using Callback = void (*)(void* cb_obj, Window& self);
		Window& callback(Callback cb, void* cb_obj);

		class Impl;
		Impl* m_impl;
		explicit Window(Impl& impl): m_impl(&impl)
		{
		}
	};
}

#endif