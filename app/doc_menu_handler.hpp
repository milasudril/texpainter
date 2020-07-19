//@	{
//@	"targets":[{"name":"doc_menu_handler.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_DOCMENUHANDLER_HPP
#define TEXPAINTER_DOCMENUHANDLER_HPP

#include "./menu_file.hpp"
#include "./size_input.hpp"
#include "model/document.hpp"
#include "ui/dialog.hpp"

namespace Texpainter
{
	template<class DocOwner>
	class DocMenuHandler
	{
		using CanvasSizeDialog = Ui::Dialog<SizeInput>;

	public:
		enum class ControlId : int
		{
			NewDocument,
			SetCanvasSize
		};

		explicit DocMenuHandler(Ui::Container& dialog_owner, DocOwner& doc_owner):
		   m_default_size{512, 512},
		   r_dlg_owner{dialog_owner},
		   r_doc_owner{doc_owner}
		{
		}

		template<auto id, class EventHandler>
		DocMenuHandler& eventHandler(EventHandler&)
		{
			return *this;
		}

		template<FileAction action>
		void onActivated(Ui::MenuItem& item)
		{
			onActivated(Tag<action>{}, item);
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


		void onActivated(Tag<FileAction::New>, Ui::MenuItem&)
		{
			// if(documentHasBeenSaved)
			{
				m_new_dlg = std::make_unique<CanvasSizeDialog>(
				   r_dlg_owner, "New document", m_default_size, Size2d{16384, 16384});
				m_new_dlg->eventHandler<ControlId::NewDocument>(*this);
			}
		}

		void onActivated(Tag<FileAction::SetCanvasSize>, Ui::MenuItem&)
		{
			m_canvas_dlg = std::make_unique<CanvasSizeDialog>
				(r_dlg_owner, "Set canvas size", m_default_size, Size2d{16384, 16384});
			m_canvas_dlg->eventHandler<ControlId::SetCanvasSize>(*this);
		}

		template<FileAction action>
		void onActivated(Tag<action>, Ui::MenuItem&)
		{
			printf("Todo %d\n", static_cast<int>(action));
		}

		void confirmPositive(Tag<ControlId::NewDocument>, CanvasSizeDialog& src)
		{
			m_default_size = src.widget().value();
			m_new_dlg.reset();
			r_doc_owner.document(Model::Document{m_default_size});
		}

		void dismiss(Tag<ControlId::NewDocument>, CanvasSizeDialog&)
		{
			m_canvas_dlg.reset();
		}

		void confirmPositive(Tag<ControlId::SetCanvasSize>, CanvasSizeDialog& src)
		{
			m_default_size = src.widget().value();
			m_canvas_dlg.reset();
			r_doc_owner.documentModify([size = m_default_size](Model::Document& doc) noexcept {
				doc.canvasSize(size);
				return true;
			});
		}

		void dismiss(Tag<ControlId::SetCanvasSize>, CanvasSizeDialog&)
		{
			m_canvas_dlg.reset();
		}

	private:
		Size2d m_default_size;

		Ui::Container& r_dlg_owner;
		DocOwner& r_doc_owner;

		std::unique_ptr<CanvasSizeDialog> m_new_dlg;
		std::unique_ptr<CanvasSizeDialog> m_canvas_dlg;

	};
}

#endif