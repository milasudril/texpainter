//@	{
//@	"targets":[{"name":"doc_menu_handler.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_APP_DOCMENUHANDLER_HPP
#define TEXPAINTER_APP_DOCMENUHANDLER_HPP

#include "./menu_file.hpp"
#include "./size_input.hpp"
#include "./document_manager.hpp"
#include "./menu_action_callback.hpp"

#include "ui/dialog.hpp"
#include "ui/error_message_dialog.hpp"
#include "utils/inherit_from.hpp"

namespace Texpainter
{
	class DocMenuHandler
	{
		using CanvasSizeDialog = Ui::Dialog<
		    InheritFrom<std::pair<std::reference_wrapper<Model::Document>, SimpleCallback>,
		                SizeInput>>;
		using NewDocDialog = Ui::Dialog<
		    InheritFrom<std::pair<std::reference_wrapper<DocumentManager>, SimpleCallback>,
		                SizeInput>>;

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

		template<auto id>
		void onActivated(Ui::MenuItem&, DocumentManager&, MenuActionCallback<id>)
		{
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

		void onActivated(Ui::MenuItem&,
		                 DocumentManager& docs,
		                 MenuActionCallback<FileAction::Export> on_completed)
		{
			if(auto current_doc = docs.currentDocument(); current_doc != nullptr)
			{
				// TODO: Move and improve this algorithm
				auto tmp = render(*current_doc, 3.0);
				PixelStore::Image img{current_doc->canvasSize()};
				for(uint32_t row = 0; row < img.width(); ++row)
				{
					for(uint32_t col = 0; col < img.height(); ++col)
					{
						img(col, row) =
						    (tmp(3 * col + 1, 3 * row + 0) + tmp(3 * col + 0, 3 * row + 1)
						     + 4.0f * tmp(3 * col + 1, 3 * row + 1) + tmp(3 * col + 2, 3 * row + 1)
						     + tmp(3 * col + 1, 3 * row + 2))
						    / 8.0f;
					}
				}
				store(img, "test.exr");
				on_completed();
			}
		}

		void onActivated(Ui::MenuItem&,
		                 DocumentManager& docs,
		                 MenuActionCallback<FileAction::New> on_completed)
		{
			// if(documentHasBeenSaved)
			{
				m_new_dlg = std::make_unique<NewDocDialog>(std::pair{std::ref(docs), on_completed},
				                                           r_dlg_owner,
				                                           "New document",
				                                           m_default_size,
				                                           Size2d{16384, 16384});
				m_new_dlg->eventHandler<ControlId::NewDocument>(*this);
			}
		}

		void onActivated(Ui::MenuItem&,
		                 DocumentManager& docs,
		                 MenuActionCallback<FileAction::SetCanvasSize> on_completed)
		{
			if(auto current_document = docs.currentDocument(); current_document != nullptr)
			{
				m_canvas_dlg = std::make_unique<CanvasSizeDialog>(
				    std::pair{std::ref(*current_document), on_completed},
				    r_dlg_owner,
				    "Set canvas size",
				    m_default_size,
				    Size2d{16384, 16384});
				m_canvas_dlg->eventHandler<ControlId::SetCanvasSize>(*this);
			}
		}

		void confirmPositive(Tag<ControlId::NewDocument>, NewDocDialog& src)
		{
			auto size      = src.widget().value();
			m_default_size = size;
			src.widget().first.get().createDocument(size);
			src.widget().second();
			m_new_dlg.reset();
		}

		void dismiss(Tag<ControlId::NewDocument>, NewDocDialog&) { m_new_dlg.reset(); }

		void confirmPositive(Tag<ControlId::SetCanvasSize>, CanvasSizeDialog& src)
		{
			auto size      = src.widget().value();
			m_default_size = size;
			src.widget().first.get().canvasSize(size);
			src.widget().second();
			m_canvas_dlg.reset();
		}

		void dismiss(Tag<ControlId::SetCanvasSize>, CanvasSizeDialog&) { m_canvas_dlg.reset(); }

		template<auto id, class DialogType>
		void handleException(char const* msg, DialogType& src) requires requires
		{
			{
				src.owner()
			}
			->std::same_as<Ui::Window&>;
		}
		{
			m_err_display.show(src.owner(), "Texpainter", msg);
		}

	private:
		Size2d m_default_size;

		Ui::Container& r_dlg_owner;

		std::unique_ptr<NewDocDialog> m_new_dlg;
		std::unique_ptr<CanvasSizeDialog> m_canvas_dlg;
		Ui::ErrorMessageDialog m_err_display;
	};
}

#endif