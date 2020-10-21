//@	{
//@	"targets":[{"name":"palette_menu_handler.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_APP_PALETTEMENUHANDLER_HPP
#define TEXPAINTER_APP_PALETTEMENUHANDLER_HPP

#include "./menu_palette.hpp"
#include "./palette_creator.hpp"
#include "./menu_action_callback.hpp"

#include "model/document.hpp"
#include "model/palette_generate.hpp"
#include "ui/dialog.hpp"
#include "ui/labeled_input.hpp"
#include "ui/text_entry.hpp"
#include "ui/error_message_dialog.hpp"
#include "utils/polymorphic_rng.hpp"
#include "utils/inherit_from.hpp"

#include <random>

namespace Texpainter
{
	class PaletteMenuHandler
	{
		using PaletteCreateDlg = Ui::Dialog<
		    InheritFrom<std::pair<std::reference_wrapper<Model::Document>, SimpleCallback>,
		                Ui::Label>,
		    Ui::DialogYesNo>;
		using PaletteGenerateDlg = Ui::Dialog<
		    InheritFrom<std::pair<std::reference_wrapper<Model::Document>, SimpleCallback>,
		                PaletteCreator>>;

	public:
		enum class ControlId : int
		{
			Clear,
			Generate
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
		void confirmNegative(Src& src)
		{
			confirmNegative(Tag<id>{}, src);
		}


		template<ControlId id, class Src>
		void confirmPositive(Src& src)
		{
			confirmPositive(Tag<id>{}, src);
		}

		void onActivated(Ui::MenuItem&, Model::Document&, MenuActionCallback<PaletteAction::Import>)
		{
		}

		void onActivated(Ui::MenuItem&, Model::Document&, MenuActionCallback<PaletteAction::Export>)
		{
		}


		void onActivated(Ui::MenuItem&,
		                 Model::Document& doc,
		                 MenuActionCallback<PaletteAction::Clear> on_completed)
		{
			if(currentLayer(doc) != nullptr)
			{
				m_new_empty_dlg = std::make_unique<PaletteCreateDlg>(
				    std::pair{std::ref(doc), on_completed},
				    r_dlg_owner,
				    "Create new palette",
				    "Are you sure you want to replace the current palette?");
				m_new_empty_dlg->eventHandler<ControlId::Clear>(*this);
			}
		}

		void onActivated(Ui::MenuItem&,
		                 Model::Document& doc,
		                 MenuActionCallback<PaletteAction::Generate> on_completed)
		{
			if(currentLayer(doc) != nullptr)
			{
				m_new_generated_dlg = std::make_unique<PaletteGenerateDlg>(
				    std::pair{std::ref(doc), on_completed}, r_dlg_owner, "Generate palette");
				m_new_generated_dlg->eventHandler<ControlId::Generate>(*this);
			}
		}

		void confirmPositive(Tag<ControlId::Clear>, PaletteCreateDlg& src)
		{
			modifyCurrentLayer(src.widget().first.get(), [](auto& layer) {
				layer.palette(Model::Layer::Palette{});
				return true;
			});
			src.widget().second();
			m_new_empty_dlg.reset();
		}

		void confirmNegative(Tag<ControlId::Clear>, PaletteCreateDlg&) { m_new_empty_dlg.reset(); }

		void confirmPositive(Tag<ControlId::Generate>, PaletteGenerateDlg& src)
		{
			auto result = src.widget().value();
			modifyCurrentLayer(src.widget().first.get(),
			                   [palette = Model::generatePalette(result.colors,
			                                                     result.by_intensity,
			                                                     result.reversed)](auto& layer) {
				                   layer.palette(palette);
				                   return true;
			                   });

			src.widget().second();
			m_new_generated_dlg.reset();
		}

		void dismiss(Tag<ControlId::Generate>, PaletteGenerateDlg&) { m_new_generated_dlg.reset(); }

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
		PolymorphicRng m_rng;
		Ui::Container& r_dlg_owner;

		std::unique_ptr<PaletteCreateDlg> m_new_empty_dlg;
		std::unique_ptr<PaletteGenerateDlg> m_new_generated_dlg;
		Ui::ErrorMessageDialog m_err_display;
	};
}

#endif