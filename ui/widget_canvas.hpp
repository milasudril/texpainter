//@	{
//@	 "targets":[{"name":"widget_canvas.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"widget_canvas.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_UI_WIDGETCANVAS_HPP
#define TEXPAINTER_UI_WIDGETCANVAS_HPP

#include "./container.hpp"

#include "utils/vec_t.hpp"

#include <utility>
#include <memory>

namespace Texpainter::Ui
{
	class WidgetCanvas: public Container
	{
	public:
		explicit WidgetCanvas(Container& parent);
		~WidgetCanvas();

		WidgetCanvas& operator=(WidgetCanvas&& obj) noexcept
		{
			std::swap(obj.m_impl, m_impl);
			return *this;
		}

		WidgetCanvas(WidgetCanvas&& obj) noexcept: m_impl(obj.m_impl) { obj.m_impl = nullptr; }

		template<class WidgetType, class... WidgetParams>
		decltype(auto) insert(vec2_t loc, WidgetParams&&... params)
		{
			return std::make_unique<WidgetType>(this->insertLocation(loc),
			                                    std::forward<WidgetParams>(params)...);
		}

		WidgetCanvas& insertLocation(vec2_t location);

		WidgetCanvas& add(void* handle) override;
		WidgetCanvas& show() override;
		WidgetCanvas& sensitive(bool val) override;
		WidgetCanvas& killFocus() override { return *this; }

		void* toplevel() const override;

	protected:
		class Impl;
		explicit WidgetCanvas(WidgetCanvas::Impl& impl): m_impl(&impl) {}
		Impl* m_impl;
	};
}

#endif
