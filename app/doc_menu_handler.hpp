//@	{
//@	"targets":[{"name":"doc_menu_handler.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_APP_DOCMENUHANDLER_HPP
#define TEXPAINTER_APP_DOCMENUHANDLER_HPP

#include "./menu_file.hpp"
#include "./size_input.hpp"
#include "./document_manager.hpp"

#include "ui/dialog.hpp"
#include "utils/inherit_from.hpp"

namespace Texpainter
{
	class DocMenuHandler
	{
		using CanvasSizeDialog =
		    Ui::Dialog<InheritFrom<std::reference_wrapper<Model::Document>, SizeInput>>;
		using NewDocDialog =
		    Ui::Dialog<InheritFrom<std::reference_wrapper<DocumentManager>, SizeInput>>;

	public:
		enum class ControlId : int
		{
			NewDocument,
			SetCanvasSize
		};

		explicit DocMenuHandler(Ui::Container& dialog_owner)
		    : m_default_size{512, 512}
		    , r_dlg_owner{dialog_owner}
		{
		}

		template<FileAction action>
		void onActivated(Ui::MenuItem& item, DocumentManager& docs)
		{
			onActivated(Tag<action>{}, item, docs);
		}

		template<ControlId id, class Src>
		void dismiss(Src& src)
		{
			dismiss(Tag<id>{}, src);
		}

		template<ControlId id, class Src>
		void confirmPositive(Src& src)
		{
			confirmPositive(Tag<id>{}, src);
		}


		void onActivated(Tag<FileAction::New>, Ui::MenuItem&, DocumentManager& docs)
		{
			// if(documentHasBeenSaved)
			{
				m_new_dlg = std::make_unique<NewDocDialog>(
				    docs, r_dlg_owner, "New document", m_default_size, Size2d{16384, 16384});
				m_new_dlg->eventHandler<ControlId::NewDocument>(*this);
			}
		}

		void onActivated(Tag<FileAction::SetCanvasSize>, Ui::MenuItem& item, DocumentManager& docs)
		{
			if(auto current_document = docs.currentDocument(); current_document != nullptr)
			{
				m_canvas_dlg = std::make_unique<CanvasSizeDialog>(*current_document,
				                                                  r_dlg_owner,
				                                                  "Set canvas size",
				                                                  m_default_size,
				                                                  Size2d{16384, 16384});
				m_canvas_dlg->eventHandler<ControlId::SetCanvasSize>(*this);
			}
			else
			{
				onActivated(Tag<FileAction::New>{}, item, docs);
			}
		}

		template<FileAction action>
		void onActivated(Tag<action>, Ui::MenuItem&, DocumentManager&)
		{
			printf("Todo %d\n", static_cast<int>(action));
		}

		void confirmPositive(Tag<ControlId::NewDocument>, NewDocDialog& src)
		{
			auto size      = src.widget().value();
			m_default_size = size;
			src.widget().get().createDocument(size);
			m_new_dlg.reset();
		}

		void dismiss(Tag<ControlId::NewDocument>, NewDocDialog&) { m_new_dlg.reset(); }

		void confirmPositive(Tag<ControlId::SetCanvasSize>, CanvasSizeDialog& src)
		{
			auto size      = src.widget().value();
			m_default_size = size;
			src.widget().get().canvasSize(size);
			m_canvas_dlg.reset();
		}

		void dismiss(Tag<ControlId::SetCanvasSize>, CanvasSizeDialog&) { m_canvas_dlg.reset(); }

	private:
		Size2d m_default_size;

		Ui::Container& r_dlg_owner;

		std::unique_ptr<NewDocDialog> m_new_dlg;
		std::unique_ptr<CanvasSizeDialog> m_canvas_dlg;
	};
}

#endif