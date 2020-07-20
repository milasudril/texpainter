//@	{
//@	"targets":[{"name":"palette_menu_handler.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_PALETTEMENUHANDLER_HPP
#define TEXPAINTER_PALETTEMENUHANDLER_HPP

#include "./menu_palette.hpp"

#include "utils/polymorphic_rng.hpp"
#include "model/document.hpp"
#include "ui/dialog.hpp"
#include "ui/labeled_input.hpp"
#include "ui/text_entry.hpp"

#include <random>

namespace Texpainter
{
	template<class DocOwner>
	class PaletteMenuHandler
	{
		using PaletteCreatorDlg = Ui::Dialog<Ui::LabeledInput<Ui::TextEntry>>;

	public:
		enum class ControlId : int
		{
			NewEmpty,
			NewGenerated
		};

		explicit PaletteMenuHandler(Ui::Container& dialog_owner, DocOwner& doc_owner, PolymorphicRng rng):
		   r_dlg_owner{dialog_owner},
		   r_doc_owner{doc_owner},
		   m_rng{rng}
		{
		}

		template<PaletteActionNew action>
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

		void onActivated(Tag<PaletteActionNew::Empty>, Ui::MenuItem&)
		{
			if(r_doc_owner.hasDocument())
			{
				m_new_empty_dlg = std::make_unique<PaletteCreatorDlg>(
				   r_dlg_owner, "Create new palette", Ui::Box::Orientation::Horizontal, "Palette name: ");
				m_new_empty_dlg->eventHandler<ControlId::NewEmpty>(*this);
			}
			else
			{
				// TODO: Create new document and retrigger this action
			}
		}

		void onActivated(Tag<PaletteActionNew::Generate>, Ui::MenuItem&)
		{
			if(r_doc_owner.hasDocument())
			{
				// TODO
			}
			else
			{
				// TODO: Create new document and retrigger this action
			}
		}

		template<PaletteActionNew action>
		void onActivated(Tag<action>, Ui::MenuItem& item)
		{
			printf("Todo: %d\n", static_cast<int>(action));
		}


		void confirmPositive(Tag<ControlId::NewEmpty>, PaletteCreatorDlg& src)
		{
			auto palette_name = src.widget().inputField().content();
			insertNewPalette(std::move(palette_name), Model::Palette{20});
			m_new_empty_dlg.reset();
		}

		void dismiss(Tag<ControlId::NewEmpty>, PaletteCreatorDlg&)
		{
			m_new_empty_dlg.reset();
		}

		void confirmPositive(Tag<ControlId::NewGenerated>, PaletteCreatorDlg&)
		{
			// TODO
			m_new_generated_dlg.reset();
		}

		void dismiss(Tag<ControlId::NewGenerated>, PaletteCreatorDlg&)
		{
			m_new_generated_dlg.reset();
		}

	private:
		Ui::Container& r_dlg_owner;
		DocOwner& r_doc_owner;
		PolymorphicRng m_rng;

		std::unique_ptr<PaletteCreatorDlg> m_new_empty_dlg;
		std::unique_ptr<PaletteCreatorDlg> m_new_generated_dlg;


		void insertNewPalette(std::string&& palette_name, Model::Palette&& palette)
		{
			r_doc_owner.documentModify([&palette_name, &palette ](auto& doc) noexcept {
				(void)doc.palettesModify([ palette_name, &palette ](auto& palettes) mutable noexcept {
					// FIXME: Unique name generator...
					palettes.insert(std::make_pair(std::move(palette_name), std::move(palette)));
					return true;
				});
				doc.currentPalette(std::move(palette_name));
				return true;
			});
		}
	};
}

#endif