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
#include <functional>

namespace Texpainter::Ui
{
	class WidgetCanvas: private Container
	{
		class WidgetDeleter;

	public:
		template<class WidgetType>
		using WidgetHandle = std::unique_ptr<WidgetType, WidgetDeleter>;

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
			auto ptr =
			    new WidgetType{this->insertLocation(loc), std::forward<WidgetParams>(params)...};
			return WidgetHandle<WidgetType>{ptr, WidgetDeleter{*m_impl}};
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

		class WidgetId
		{
		public:
			constexpr WidgetId():m_value{0}{}

			constexpr auto operator<=>(WidgetId const&) const = default;

			constexpr WidgetId& operator++()
			{
				++m_value;
				return *this;
			}

		private:
			uint64_t m_value;
		};

	private:
		class WidgetDeleter
		{
		public:
			explicit WidgetDeleter(std::reference_wrapper<Impl> impl) noexcept;

			template<class WidgetType>
			void operator()(WidgetType* widget) noexcept
			{
				delete widget;
				do_cleanup();
			}

		private:
			std::reference_wrapper<Impl> r_impl;
			WidgetId m_id;
			void do_cleanup() noexcept;
		};
	};
}

#endif
