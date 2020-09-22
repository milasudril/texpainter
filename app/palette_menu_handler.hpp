//@	{
//@	"targets":[{"name":"palette_menu_handler.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_APP_PALETTEMENUHANDLER_HPP
#define TEXPAINTER_APP_PALETTEMENUHANDLER_HPP

#include "./menu_palette.hpp"
#include "./palette_creator.hpp"
#include "./menu_action_callback.hpp"

#include "model/document.hpp"
#include "ui/dialog.hpp"
#include "ui/labeled_input.hpp"
#include "ui/text_entry.hpp"
#include "utils/polymorphic_rng.hpp"
#include "utils/inherit_from.hpp"

#include <random>

namespace Texpainter
{
	class PaletteMenuHandler
	{
		using PaletteCreateDlg = Ui::Dialog<
		    InheritFrom<std::pair<std::reference_wrapper<Model::Document>, SimpleCallback>,
		                Ui::LabeledInput<Ui::TextEntry>>>;
		using PaletteGenerateDlg = Ui::Dialog<
		    InheritFrom<std::pair<std::reference_wrapper<Model::Document>, SimpleCallback>,
		                PaletteCreator>>;

	public:
		enum class ControlId : int
		{
			NewEmpty,
			NewGenerated
		};

		explicit PaletteMenuHandler(Ui::Container& dialog_owner, PolymorphicRng rng)
		    : m_rng{rng}
		    , r_dlg_owner{dialog_owner}
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
		                 Model::Document& doc,
		                 MenuActionCallback<PaletteActionNew::Empty> on_completed)
		{
			m_new_empty_dlg =
			    std::make_unique<PaletteCreateDlg>(std::pair{std::ref(doc), on_completed},
			                                       r_dlg_owner,
			                                       "Create new palette",
			                                       Ui::Box::Orientation::Horizontal,
			                                       "Palette name: ");
			m_new_empty_dlg->eventHandler<ControlId::NewEmpty>(*this);
		}

		void onActivated(Ui::MenuItem&,
		                 Model::Document& doc,
		                 MenuActionCallback<PaletteActionNew::Generate> on_completed)
		{
			m_new_generated_dlg = std::make_unique<PaletteGenerateDlg>(
			    std::pair{std::ref(doc), on_completed}, r_dlg_owner, "Generate palette");
			m_new_generated_dlg->eventHandler<ControlId::NewGenerated>(*this);
		}

		void confirmPositive(Tag<ControlId::NewEmpty>, PaletteCreateDlg& src)
		{
			auto palette_name = src.widget().inputField().content();
			insertNewPalette(
			    std::move(palette_name), PixelStore::Palette{23}, src.widget().first.get());
			src.widget().second();
			m_new_empty_dlg.reset();
		}

		void dismiss(Tag<ControlId::NewEmpty>, PaletteCreateDlg&) { m_new_empty_dlg.reset(); }

		void confirmPositive(Tag<ControlId::NewGenerated>, PaletteGenerateDlg& src)
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
			insertNewPalette(
			    std::move(palette_info.name), std::move(pal), src.widget().first.get());
			src.widget().second();
			m_new_generated_dlg.reset();
		}

		void dismiss(Tag<ControlId::NewGenerated>, PaletteGenerateDlg&)
		{
			m_new_generated_dlg.reset();
		}

	private:
		PolymorphicRng m_rng;
		Ui::Container& r_dlg_owner;

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