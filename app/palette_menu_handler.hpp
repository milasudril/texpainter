//@	{
//@	"targets":[{"name":"palette_menu_handler.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_APP_PALETTEMENUHANDLER_HPP
#define TEXPAINTER_APP_PALETTEMENUHANDLER_HPP

#include "./menu_palette.hpp"
#include "./palette_creator.hpp"

#include "utils/polymorphic_rng.hpp"
#include "model/document.hpp"
#include "ui/dialog.hpp"
#include "ui/labeled_input.hpp"
#include "ui/text_entry.hpp"

#include <random>

namespace Texpainter
{
	class PaletteMenuHandler
	{
		template<class DialogType>
		class InputDialog
		{
		public:
			template<class... Args>
			explicit InputDialog(Model::Document& doc, Args&&... args)
			    : r_document{&doc}
			    , m_dlg{std::forward<Args>(args)...} {};

			template<auto id, class EventHandler>
			InputDialog& eventHandler(EventHandler& eh)
			{
				m_dlg.template eventHandler<id>(eh);
				return *this;
			}

			auto& widget() { return m_dlg.widget(); }

			Model::Document& document() { return *r_document; }

		private:
			Model::Document* r_document;
			DialogType m_dlg;
		};

		using PaletteCreateDlg   = InputDialog<Ui::Dialog<Ui::LabeledInput<Ui::TextEntry>>>;
		using PaletteGenerateDlg = InputDialog<Ui::Dialog<PaletteCreator>>;

	public:
		enum class ControlId : int
		{
			NewEmpty,
			NewGenerated
		};

		explicit PaletteMenuHandler(Ui::Container& dialog_owner, PolymorphicRng rng)
		    : r_dlg_owner{dialog_owner}
		    , m_rng{rng}
		{
		}

		template<PaletteActionNew action>
		void onActivated(Ui::MenuItem& item, Model::Document& doc)
		{
			onActivated(Tag<action>{}, item, doc);
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

		void onActivated(Tag<PaletteActionNew::Empty>, Ui::MenuItem&, Model::Document& doc)
		{
			m_new_empty_dlg = std::make_unique<PaletteCreateDlg>(doc,
			                                                     r_dlg_owner,
			                                                     "Create new palette",
			                                                     Ui::Box::Orientation::Horizontal,
			                                                     "Palette name: ");
			m_new_empty_dlg->eventHandler<ControlId::NewEmpty>(*this);
		}

		void onActivated(Tag<PaletteActionNew::Generate>, Ui::MenuItem&, Model::Document& doc)
		{
			m_new_generated_dlg =
			    std::make_unique<PaletteGenerateDlg>(doc, r_dlg_owner, "Generate palette");
			m_new_generated_dlg->eventHandler<ControlId::NewGenerated>(*this);
		}

		template<PaletteActionNew action>
		void onActivated(Tag<action>, Ui::MenuItem& item)
		{
			printf("Todo: %d\n", static_cast<int>(action));
		}


		template<class Dialog>
		void confirmPositive(Tag<ControlId::NewEmpty>, Dialog& src)
		{
			auto palette_name = src.widget().inputField().content();
			insertNewPalette(std::move(palette_name), PixelStore::Palette{23}, src.document());
			m_new_empty_dlg.reset();
		}

		template<class Dialog>
		void dismiss(Tag<ControlId::NewEmpty>, Dialog&)
		{
			m_new_empty_dlg.reset();
		}


		template<class Dialog>
		void confirmPositive(Tag<ControlId::NewGenerated>, Dialog& src)
		{
			auto palette_info = src.widget().value();
			PixelStore::Palette pal{23};
			std::ranges::for_each(palette_info.colors, [&pal, k = 0u](auto val) mutable {
				auto hsi = toHsi(val);
				for(int l = 0; l < 5; ++l)
				{
					pal[PixelStore::ColorIndex{k}] = toRgb(hsi);
					++k;
					hsi.intensity /= 2.0f;
				}
			});
			pal[PixelStore::ColorIndex{20}] = toRgb(PixelStore::Hsi{0.0f, 0.0f, 1.0f / 3.0, 1.0f});
			pal[PixelStore::ColorIndex{21}] =
			    toRgb(PixelStore::Hsi{0.0f, 0.0f, 1.0f / (128.0f * 3.0), 1.0f});
			pal[PixelStore::ColorIndex{22}] = PixelStore::Pixel{0.0f, 0.0f, 0.0f, 0.0f};
			insertNewPalette(std::move(palette_info.name), std::move(pal), src.document());
			m_new_generated_dlg.reset();
		}

		template<class Dialog>
		void dismiss(Tag<ControlId::NewGenerated>, Dialog&)
		{
			m_new_generated_dlg.reset();
		}

	private:
		Ui::Container& r_dlg_owner;
		PolymorphicRng m_rng;

		std::unique_ptr<PaletteCreateDlg> m_new_empty_dlg;
		std::unique_ptr<PaletteGenerateDlg> m_new_generated_dlg;


		void insertNewPalette(std::string&& palette_name,
		                      PixelStore::Palette&& palette,
		                      Model::Document& doc)
		{
			doc.palettesModify([palette_name, &palette](auto& palettes) mutable noexcept {
				// FIXME: Unique name generator...
				palettes.insert(std::make_pair(std::move(palette_name), std::move(palette)));
				return true;
			});
			doc.currentPalette(std::move(palette_name));
		}
	};
}

#endif