//@	{
//@	 "targets":[{"name":"palette_view.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"palette_view.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_UI_PALETTEVIEW_HPP
#define TEXPAINTER_UI_PALETTEVIEW_HPP

#include "./container.hpp"
#include "./dispatch_event.hpp"

#include "pixel_store/rgba_value.hpp"
#include "pixel_store/color_index.hpp"
#include "utils/size_2d.hpp"

#include <utility>
#include <span>

namespace Texpainter::Ui
{
	class PaletteView
	{
	public:
		explicit PaletteView(Container& container);
		~PaletteView();

		PaletteView& operator=(PaletteView&& obj) noexcept
		{
			std::swap(obj.m_impl, m_impl);
			return *this;
		}

		PaletteView(PaletteView&& obj) noexcept: m_impl(obj.m_impl) { obj.m_impl = nullptr; }

		template<auto id, class EventHandler>
		PaletteView& eventHandler(EventHandler& eh)
		{
			return eventHandler(
			    &eh,
			    {[](void* event_handler,
			        PaletteView& self,
			        PixelStore::ColorIndex index,
			        int button) {
				     auto& obj = *reinterpret_cast<EventHandler*>(event_handler);
				     obj.template onMouseDown<id>(self, index, button);
			     },
			     [](void* event_handler,
			        PaletteView& self,
			        PixelStore::ColorIndex index,
			        int button) {
				     auto& obj = *reinterpret_cast<EventHandler*>(event_handler);
				     dispatchEvent<id>(
				         [](EventHandler& eh, auto&&... args) {
					         eh.template onMouseUp<id>(std::forward<decltype(args)>(args)...);
				         },
				         obj,
				         self,
				         index,
				         button);
			     },
			     [](void* event_handler, PaletteView& self, PixelStore::ColorIndex index) {
				     auto& obj = *reinterpret_cast<EventHandler*>(event_handler);
				     obj.template onMouseMove<id>(self, index);
			     }});
		}

		PaletteView& palette(std::span<PixelStore::RgbaValue const> predef_colors);
		std::span<PixelStore::RgbaValue const> palette() const;
		PaletteView& update();

		enum class HighlightMode
		{
			None,
			Read,
			Write
		};

		PaletteView& color(PixelStore::ColorIndex index, PixelStore::RgbaValue value);
		PixelStore::RgbaValue color(PixelStore::ColorIndex index) const;

		PaletteView& highlightMode(PixelStore::ColorIndex index, HighlightMode mode);

		PaletteView& minSize(Size2d size);

	private:
		class Impl;
		explicit PaletteView(Impl& impl): m_impl(&impl) {}
		Impl* m_impl;
		struct EventHandlerVtable
		{
			void (*m_on_mouse_down)(void* event_handler,
			                        PaletteView& self,
			                        PixelStore::ColorIndex index,
			                        int button);
			void (*m_on_mouse_up)(void* event_handler,
			                      PaletteView& self,
			                      PixelStore::ColorIndex index,
			                      int button);
			void (*m_on_mouse_move)(void* event_handler,
			                        PaletteView& self,
			                        PixelStore::ColorIndex index);
		};
		PaletteView& eventHandler(void* event_handler, EventHandlerVtable const& vtable);
	};
}

#endif