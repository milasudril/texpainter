//@	{
//@	 "targets":[{"name":"widget_multiplexer.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"./widget_multiplexer.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_UI_WIDGETMULTIPLEXER_HPP
#define TEXPAINTER_UI_WIDGETMULTIPLEXER_HPP

#include "./container.hpp"

#include <utility>

namespace Texpainter::Ui
{
	class WidgetMultiplexer: public Container
	{
	public:
		explicit WidgetMultiplexer(Container& parent);
		~WidgetMultiplexer() override;

		WidgetMultiplexer& operator=(WidgetMultiplexer&& obj) noexcept
		{
			std::swap(obj.m_impl, m_impl);
			return *this;
		}

		WidgetMultiplexer(WidgetMultiplexer&& obj) noexcept: m_impl(obj.m_impl)
		{
			obj.m_impl = nullptr;
		}

		WidgetMultiplexer& add(void* handle) override;
		WidgetMultiplexer& show() override;
		WidgetMultiplexer& sensitive(bool val) override;
		WidgetMultiplexer& killFocus() override;

		void* toplevel() const override;

		WidgetMultiplexer& widgetName(char const*);

		WidgetMultiplexer& showWidget(char const* widget_name);

	protected:
		class Impl;
		explicit WidgetMultiplexer(WidgetMultiplexer::Impl& impl): m_impl(&impl) {}
		Impl* m_impl;
	};
}

#endif
