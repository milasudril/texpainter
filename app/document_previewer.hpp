//@	{"targets":[{"name":"document_previewer.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_DOCUMENTPREVIEWER_HPP
#define TEXPAINTER_APP_DOCUMENTPREVIEWER_HPP

#include "model/document.hpp"

#include "ui/image_view.hpp"
#include "ui/glarea.hpp"
#include "ui/keyboard_state.hpp"
#include "ui/widget_multiplexer.hpp"

#include <utility>
#include <functional>

namespace Texpainter::App
{
	class DocumentPreviewer
	{
		enum class ControlId : int
		{
			DocumentView
		};

	public:
		DocumentPreviewer(DocumentPreviewer&&) = delete;

		explicit DocumentPreviewer(Ui::Container& owner, Model::Document& doc)
		    : m_doc{doc}
		    , m_views{owner}
		    , m_img_view{m_views.widgetName("imgview")}
		    , m_gl_area{m_views.widgetName("glarea")}
		{
			m_img_view.scale(0.5);
			m_views.showWidget("glarea");
			refresh();
		}

		DocumentPreviewer& refreshImageView(
		    Model::Document::ForceUpdate force_update = Model::Document::ForceUpdate{true})
		{
			auto result = render(m_doc.get(), force_update);
			m_img_view.image(result.pixels());
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

	private:
		std::reference_wrapper<Model::Document> m_doc;
		void* r_eh;
		void (*on_updated)(void*, DocumentPreviewer&);

		template<auto>
		void onUpdated(DocumentPreviewer& src)
		{
			src.refresh();
		}

		Ui::WidgetMultiplexer m_views;
		Ui::ImageView m_img_view;
		Ui::GLArea m_gl_area;
	};
}

#endif