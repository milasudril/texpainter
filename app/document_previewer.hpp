//@	{"targets":[{"name":"document_preview.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_DOCUMENTPREVIEWER_HPP
#define TEXPAINTER_APP_DOCUMENTPREVIEWER_HPP

#include "model/document.hpp"

#include "ui/image_view.hpp"
#include "ui/keyboard_state.hpp"

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
		    , m_img_view{owner}
		{
			refresh();
		}

		DocumentPreviewer& refreshImageView()
		{
			auto result = render(m_doc.get());
			m_img_view.image(result.pixels());
			return *this;
		}

		DocumentPreviewer& refresh() { return refreshImageView(); }

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

		Ui::ImageView m_img_view;
	};
}

#endif