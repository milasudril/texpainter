//@	{
//@	 "targets":[{"name":"widget_canvas.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"widget_canvas.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_UI_WIDGETCANVAS_HPP
#define TEXPAINTER_UI_WIDGETCANVAS_HPP

#include "./container.hpp"
#include "./widget_coordinates.hpp"
#include "./screen_coordinates.hpp"

#include <utility>
#include <memory>
#include <functional>

namespace Texpainter::Ui
{
	class WidgetCanvasDetail: private Container
	{
		class WidgetDeleter;

	protected:
		class ClientId;

	public:
		template<class WidgetType>
		using WidgetHandle = std::unique_ptr<WidgetType, WidgetDeleter>;

		explicit WidgetCanvasDetail(Container& parent);
		~WidgetCanvasDetail();

		WidgetCanvasDetail& operator=(WidgetCanvasDetail&& obj) noexcept
		{
			std::swap(obj.m_impl, m_impl);
			return *this;
		}

		WidgetCanvasDetail(WidgetCanvasDetail&& obj) noexcept: m_impl(obj.m_impl)
		{
			obj.m_impl = nullptr;
		}

		WidgetCanvasDetail& insertLocation(WidgetCoordinates location);
		WidgetCanvasDetail& clientId(ClientId id);

		WidgetCanvasDetail& add(void* handle) override;
		WidgetCanvasDetail& show() override;
		WidgetCanvasDetail& sensitive(bool val) override;
		WidgetCanvasDetail& killFocus() override { return *this; }


		void* toplevel() const override;

		Container& asContainer() { return *this; }

	protected:
		class Impl;
		explicit WidgetCanvasDetail(WidgetCanvasDetail::Impl& impl): m_impl(&impl) {}
		Impl* m_impl;

		class WidgetId
		{
		public:
			constexpr WidgetId(): m_value{0} {}

			constexpr auto operator<=>(WidgetId const&) const = default;

			constexpr WidgetId& operator++()
			{
				++m_value;
				return *this;
			}

		private:
			uint64_t m_value;
		};

		class ClientId
		{
		public:
			ClientId() = default;

			template<class T>
			constexpr explicit ClientId(T val) requires(
			    (std::is_integral_v<T> || std::is_enum_v<T>)&&sizeof(val) <= sizeof(size_t))
			    : m_value{val}
			{
			}

			template<class T>
			constexpr explicit ClientId(T val) requires (sizeof(val)<=sizeof(size_t) && requires(T x){{x.value()}->std::convertible_to<size_t>;}):
			m_value{val.value()}
			{
			}

			constexpr size_t value() const { return m_value; }

		private:
			size_t m_value;
		};

		using EventHandlerFunc = void (*)(
		    void*, WidgetCanvasDetail&, WidgetCoordinates, ScreenCoordinates, int, ClientId);

		void eventHandler(void*, EventHandlerFunc);

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

	template<class ClientIdType>
	class WidgetCanvas: private WidgetCanvasDetail
	{
	public:
		using WidgetCanvasDetail::WidgetCanvasDetail;

		template<class WidgetType>
		using WidgetHandle = WidgetCanvasDetail::WidgetHandle<WidgetType>;

		template<class WidgetType, class... WidgetParams>
		decltype(auto) insert(ClientIdType id, WidgetCoordinates loc, WidgetParams&&... params)
		{
			auto ptr = new WidgetType{clientId(ClientId{id}).insertLocation(loc).asContainer(),
			                          std::forward<WidgetParams>(params)...};
			return WidgetHandle<WidgetType>{ptr, WidgetDeleter{*m_impl}};
		}

		template<auto id, class EventHandler>
		WidgetCanvas& eventHandler(EventHandler& eh)
		{
			WidgetCanvasDetail::eventHandler(
			    &eh,
			    [](void* event_handler,
			       WidgetCanvasDetail& self,
			       WidgetCoordinates loc_window,
			       ScreenCoordinates loc_screen,
			       int button,
			       ClientId widget) {
				    auto& obj = *reinterpret_cast<EventHandler*>(event_handler);
				    obj.template onMouseDown<id>(static_cast<WidgetCanvas&>(self),
				                                 loc_window,
				                                 loc_screen,
				                                 button,
				                                 ClientIdType{widget.value()});
			    });
			return *this;
		}
	};
}

#endif
