//@	{
//@	 "targets":[{"name":"widget_canvas.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"widget_canvas.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_UI_WIDGETCANVAS_HPP
#define TEXPAINTER_UI_WIDGETCANVAS_HPP

#include "./container.hpp"
#include "./widget_coordinates.hpp"
#include "./screen_coordinates.hpp"
#include "./toplevel_coordinates.hpp"

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
		enum class InsertMode : int
		{
			Fixed,
			Movable
		};

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
		WidgetCanvasDetail& insertMode(InsertMode mode);
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

		struct EventHandlerVtable
		{
			void (*on_move_canvas)(void*, WidgetCanvasDetail&, ToplevelCoordinates);
			void (*on_move)(void*, WidgetCanvasDetail&, WidgetCoordinates, ClientId);
			void (*on_mouse_down)(
			    void*, WidgetCanvasDetail&, WidgetCoordinates, ScreenCoordinates, int, ClientId);
			void (*on_mouse_up_canvas)(
			    void*, WidgetCanvasDetail&, WidgetCoordinates, ScreenCoordinates, int);
			void (*on_realized)(void*, WidgetCanvasDetail&);
		};

		void eventHandler(void*, EventHandlerVtable const& vt);

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
			ClientId m_id;
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
			auto ptr = new WidgetType{clientId(ClientId{id})
			                              .insertLocation(loc)
			                              .insertMode(InsertMode::Movable)
			                              .asContainer(),
			                          std::forward<WidgetParams>(params)...};
			return WidgetHandle<WidgetType>{ptr, WidgetDeleter{*m_impl}};
		}

		template<class WidgetType, class... WidgetParams>
		decltype(auto) insert(WidgetParams&&... params)
		{
			return std::make_unique<WidgetType>(insertMode(InsertMode::Fixed).asContainer(),
			                                    std::forward<WidgetParams>(params)...);
		}

		void showWidgets() { WidgetCanvasDetail::show(); }

		template<auto id, class EventHandler>
		WidgetCanvas& eventHandler(EventHandler& eh)
		{
			WidgetCanvasDetail::eventHandler(
			    &eh,
			    EventHandlerVtable{
			        [](void* event_handler, WidgetCanvasDetail& self, ToplevelCoordinates loc) {
				        auto& obj = *reinterpret_cast<EventHandler*>(event_handler);
				        obj.template onMouseMove<id>(static_cast<WidgetCanvas&>(self), loc);
			        },
			        [](void* event_handler,
			           WidgetCanvasDetail& self,
			           WidgetCoordinates loc_window,
			           ClientId widget) {
				        auto& obj = *reinterpret_cast<EventHandler*>(event_handler);
				        obj.template onMove<id>(static_cast<WidgetCanvas&>(self),
				                                loc_window,
				                                ClientIdType{widget.value()});
			        },
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
			        },
			        [](void* event_handler,
			           WidgetCanvasDetail& self,
			           WidgetCoordinates loc_window,
			           ScreenCoordinates loc_screen,
			           int button) {
				        auto& obj = *reinterpret_cast<EventHandler*>(event_handler);
				        obj.template onMouseUp<id>(
				            static_cast<WidgetCanvas&>(self), loc_window, loc_screen, button);
			        },
			        [](void* event_handler, WidgetCanvasDetail& self) {
				        auto& obj = *reinterpret_cast<EventHandler*>(event_handler);
				        obj.template onRealized<id>(static_cast<WidgetCanvas&>(self));
			        }});
			return *this;
		}
	};
}

#endif
