//@	{
//@  "targets":[{"name":"palette_editor.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_PALETTEEDITOR_HPP
#define TEXPAINTER_PALETTEEDITOR_HPP

#include "ui/box.hpp"
#include "ui/combobox.hpp"
#include "ui/palette_view.hpp"
#include "ui/button.hpp"
#include "ui/labeled_input.hpp"
#include "ui/text_entry.hpp"
#include "ui/dialog.hpp"
#include "ui/extended_color_picker.hpp"
#include "ui/color_picker_sidepanel.hpp"
#include "model/palette.hpp"

#include <vector>

namespace Texpainter
{
	class PaletteEditor
	{
		enum class ControlId : int
		{
			PalSelector,
			PaletteCreate,
			PaletteCreateDlg,
			PalView,
			ColorPicker
		};

		using PaletteNameInput = Texpainter::Ui::Dialog<Ui::LabeledInput<Ui::TextEntry>>;

		using ColorPicker = Texpainter::Ui::Dialog<
		   Texpainter::Ui::ExtendedColorPicker<Texpainter::Ui::ColorPickerSidepanel>>;


	public:
		explicit PaletteEditor(Ui::Container& owner):
		   m_sel_color_index{0},
		   m_container{owner, Ui::Box::Orientation::Horizontal},
		   m_pal_selector{m_container},
		   m_pal_new{m_container, "+"},
		   m_pal_view{m_container.insertMode(Ui::Box::InsertMode{4})}
		{
			m_pal_selector.eventHandler<ControlId::PalSelector>(*this);
			m_pal_new.eventHandler<ControlId::PaletteCreate>(*this);
			m_pal_view.eventHandler<ControlId::PalView>(*this);
		}

		void createPalette(char const* name)
		{
			m_pal_selector.append(name);
			m_palettes.push_back(Model::Palette{20});
			select(m_palettes.size() - 1);
		}

		void select(size_t index)
		{
			m_pal_selector.selected(index);
			m_pal_view.palette(m_palettes[index])
			   .highlightMode(0, Texpainter::Ui::PaletteView::HighlightMode::Read);
			m_sel_color_index = 0;
		}

		Model::Palette const& selectedPalette() const
		{
			return m_palettes[m_pal_selector.selected()];
		}

		size_t paletteCount() const
		{
			return m_palettes.size();
		}

		template<ControlId>
		void onChanged(Ui::Combobox&);

		template<ControlId>
		void onClicked(Ui::Button&);

		template<ControlId>
		void dismiss(PaletteNameInput&);

		template<ControlId>
		void confirmPositive(PaletteNameInput&);

		template<ControlId>
		void onMouseDown(Texpainter::Ui::PaletteView& view, size_t, int);

		template<ControlId>
		void onMouseUp(Texpainter::Ui::PaletteView&, size_t, int)
		{
		}

		template<ControlId>
		void onMouseMove(Texpainter::Ui::PaletteView&, size_t)
		{
		}

		template<ControlId>
		void dismiss(ColorPicker&);

		template<ControlId>
		void confirmPositive(ColorPicker&);


	private:
		std::vector<Model::Palette> m_palettes;
		uint32_t m_sel_color_index;
		uint32_t m_modified_pal_index;

		Ui::Box m_container;
		Ui::Combobox m_pal_selector;
		Ui::Button m_pal_new;
		Ui::PaletteView m_pal_view;
		std::unique_ptr<PaletteNameInput> m_pal_name_input;
		std::unique_ptr<ColorPicker> m_color_picker;
	};

	template<>
	void PaletteEditor::onChanged<PaletteEditor::ControlId::PalSelector>(Ui::Combobox& box)
	{
		m_pal_view.palette(m_palettes[m_pal_selector.selected()])
		   .highlightMode(0, Texpainter::Ui::PaletteView::HighlightMode::Read);
		m_sel_color_index = 0;
	}

	template<>
	void PaletteEditor::onClicked<PaletteEditor::ControlId::PaletteCreate>(Ui::Button& btn)
	{
		m_pal_name_input = std::make_unique<PaletteNameInput>(
		   m_container, "Create a new palette", Ui::Box::Orientation::Horizontal, "Enter palette name: ");
		m_pal_name_input->eventHandler<ControlId::PaletteCreateDlg>(*this).widget().inputField().focus();
	}

	template<>
	void PaletteEditor::dismiss<PaletteEditor::ControlId::PaletteCreateDlg>(PaletteNameInput&)
	{
		m_pal_name_input.reset();
		m_pal_new.state(false);
	}

	template<>
	void PaletteEditor::confirmPositive<PaletteEditor::ControlId::PaletteCreateDlg>(
	   PaletteNameInput& dlg)
	{
		createPalette(dlg.widget().inputField().content());
		m_pal_name_input.reset();
		m_pal_new.state(false);
	}

	template<>
	void PaletteEditor::onMouseDown<PaletteEditor::ControlId::PalView>(
	   Texpainter::Ui::PaletteView& view, size_t index, int button)
	{
		if(index < std::size(selectedPalette()))
		{
			switch(button)
			{
				case 3:
				{
					m_modified_pal_index = static_cast<uint32_t>(index);
					m_pal_view.highlightMode(index, Texpainter::Ui::PaletteView::HighlightMode::Write);
					m_color_picker = std::make_unique<ColorPicker>(
					   m_container, (std::string{"Select color number "} + std::to_string(index + 1)).c_str());
					m_color_picker->eventHandler<ControlId::ColorPicker>(*this).widget().value(
					   selectedPalette()[index]);
				}
				break;

				case 1:
					m_pal_view.highlightMode(m_sel_color_index, Texpainter::Ui::PaletteView::HighlightMode::None)
					   .highlightMode(index, Texpainter::Ui::PaletteView::HighlightMode::Read);
					m_sel_color_index = index;
					break;
			}
		}
	}

	template<>
	void PaletteEditor::dismiss<PaletteEditor::ControlId::ColorPicker>(ColorPicker&)
	{
		m_pal_view.highlightMode(m_modified_pal_index, Texpainter::Ui::PaletteView::HighlightMode::None)
		   .highlightMode(m_sel_color_index, Texpainter::Ui::PaletteView::HighlightMode::Read);
		m_color_picker.reset();
	}

	template<>
	void PaletteEditor::confirmPositive<PaletteEditor::ControlId::ColorPicker>(ColorPicker& picker)
	{
		auto color_new = picker.widget().sidepanel().targetIntensity()(picker.widget().value());
		m_palettes[m_pal_selector.selected()][m_modified_pal_index] = color_new;

		m_pal_view.highlightMode(m_modified_pal_index, Texpainter::Ui::PaletteView::HighlightMode::None)
		   .highlightMode(m_sel_color_index, Texpainter::Ui::PaletteView::HighlightMode::Read);
		m_color_picker.reset();
	}
}

#endif