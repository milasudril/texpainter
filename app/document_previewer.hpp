//@	{"targets":[{"name":"document_previewer.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_DOCUMENTPREVIEWER_HPP
#define TEXPAINTER_APP_DOCUMENTPREVIEWER_HPP

#include "./terrain_view.hpp"

#include "model/document.hpp"

#include "ui/image_view.hpp"
#include "ui/glarea.hpp"
#include "ui/keyboard_state.hpp"
#include "ui/widget_multiplexer.hpp"
#include "ui/box.hpp"
#include "ui/combobox.hpp"

#include <utility>
#include <functional>

namespace Texpainter::App
{
	class DocumentPreviewer
	{
		enum class ControlId : int
		{
			ViewSelector,
		};

	public:
		DocumentPreviewer(DocumentPreviewer&&) = delete;

		explicit DocumentPreviewer(Ui::Container& owner, Model::Document& doc)
		    : m_doc{doc}
		    , m_root{owner, Ui::Box::Orientation::Vertical}
		    , m_view_selector{m_root}
		    , m_views{m_root.insertMode(Ui::Box::InsertMode{0, Ui::Box::Fill | Ui::Box::Expand})}
		    , m_img_view{m_views.widgetName("imgview")}
		    , m_terrain_view{m_views.widgetName("terrainview")}
		{
			m_img_view.scale(0.5);
			m_view_selector.append("imgview")
			    .append("terrainview")
			    .selected(0)
			    .eventHandler<ControlId::ViewSelector>(*this);
			refresh();
		}

		DocumentPreviewer& refreshImageView(
		    Model::Document::ForceUpdate force_update = Model::Document::ForceUpdate{true})
		{
			auto result = render(m_doc.get(), force_update);
			m_img_view.image(result.pixels());
			m_terrain_view.meshSize(result.pixels().size());
			return *this;
		}

		DocumentPreviewer& refresh(
		    Model::Document::ForceUpdate force_update = Model::Document::ForceUpdate{true})
		{
			return refreshImageView(force_update);
		}

		void onKeyDown(Ui::KeyboardState const&) {}

		template<auto id, class EventHandler>
		DocumentPreviewer& eventHandler(EventHandler& eh)
		{
			r_eh       = &eh;
			on_updated = [](void* self, DocumentPreviewer& src) {
				reinterpret_cast<EventHandler*>(self)->template onUpdated<id>(src);
			};
			return *this;
		}

		template<ControlId>
		void onChanged(Ui::Combobox& src)
		{
			if(src.selected() == 0) { m_views.showWidget("imgview"); }
			else
			{
				m_views.showWidget("terrainview");
				m_terrain_view.meshSize(m_doc.get().canvasSize());
			}
		}

		template<ControlId, class... T>
		void handleException(const T&...)
		{
		}

	private:
		std::reference_wrapper<Model::Document> m_doc;
		void* r_eh;
		void (*on_updated)(void*, DocumentPreviewer&);

		template<auto>
		void onUpdated(DocumentPreviewer& src)
		{
			src.refresh();
		}

		Ui::Box m_root;
		Ui::Combobox m_view_selector;
		Ui::WidgetMultiplexer m_views;
		Ui::ImageView m_img_view;
		TerrainView m_terrain_view;
	};
}

#endif