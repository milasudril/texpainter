//@	{
//@  "targets":[{"name":"palette_editor.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_PALETTEEDITOR_HPP
#define TEXPAINTER_PALETTEEDITOR_HPP

#include "./unique_name.hpp"

#include "ui/box.hpp"
#include "ui/combobox.hpp"
#include "ui/palette_view.hpp"
#include "ui/labeled_input.hpp"
#include "ui/text_entry.hpp"
#include "ui/dialog.hpp"
#include "ui/color_picker.hpp"
#include "ui/separator.hpp"
#include "model/palette.hpp"
#include "pcg-cpp/include/pcg_random.hpp"

#include <vector>
#include <unordered_set>
#include <string>

namespace Texpainter
{
	class PaletteEditor
	{
		enum class ControlId : int
		{
			PalSelector,
			PalCreateEmpty,
			PalView,
			ColorPicker
		};

		using PaletteNameInput = Texpainter::Ui::Dialog<Ui::LabeledInput<Ui::TextEntry>>;

		using ColorPicker = Texpainter::Ui::Dialog<Texpainter::Ui::ColorPicker>;


	public:
		explicit PaletteEditor(Ui::Container& owner)
		    : m_sel_color_index{0}
		    , r_eh{nullptr}
		    , m_container{owner, Ui::Box::Orientation::Horizontal}
		    , m_pal_selector{m_container}
		    , m_pal_view{
		          m_container.insertMode(Ui::Box::InsertMode{2, Ui::Box::Fill | Ui::Box::Expand})}
		{
			m_pal_selector.eventHandler<ControlId::PalSelector>(*this);
			m_pal_view.eventHandler<ControlId::PalView>(*this);
			m_used_pal_names.reserve(13);
		}

		void createEmptyPalette()
		{
			m_pal_name_input =
			    std::make_unique<PaletteNameInput>(m_container,
			                                       "Create new palette",
			                                       Texpainter::Ui::Box::Orientation::Horizontal,
			                                       "Palette name:");
			m_pal_name_input->eventHandler<ControlId::PalCreateEmpty>(*this)
			    .widget()
			    .inputField()
			    .focus();
		}

		PaletteEditor& createEmptyPalette(char const* name)
		{
			m_pal_selector.append(generateEntryName(name, m_used_pal_names).c_str());
			m_palettes.push_back(Model::Palette{20});
			select(m_palettes.size() - 1);
			return *this;
		}

		PaletteEditor& select(size_t index)
		{
			m_pal_selector.selected(index);
			m_pal_view.palette(m_palettes[index])
			    .highlightMode(Model::ColorIndex{0},
			                   Texpainter::Ui::PaletteView::HighlightMode::Read);
			m_sel_color_index = Model::ColorIndex{0};
			return *this;
		}

		Model::Palette const& selectedPalette() const
		{
			return m_palettes[m_pal_selector.selected()];
		}

		template<auto id, class EventHandler>
		PaletteEditor& eventHandler(EventHandler& eh)
		{
			r_eh   = &eh;
			r_func = [](void* event_handler, PaletteEditor& self) {
				reinterpret_cast<EventHandler*>(event_handler)->template onChanged<id>(self);
			};

			return *this;
		}

		size_t paletteCount() const { return m_palettes.size(); }

		Model::ColorIndex selectedColorIndex() const { return m_sel_color_index; }

		template<ControlId>
		void onChanged(Ui::Combobox&);

		template<ControlId>
		void onMouseDown(Texpainter::Ui::PaletteView& view, Model::ColorIndex, int)
		{
		}

		template<ControlId>
		void onMouseUp(Texpainter::Ui::PaletteView&, Model::ColorIndex, int);

		template<ControlId>
		void onMouseMove(Texpainter::Ui::PaletteView&, Model::ColorIndex)
		{
		}

		template<ControlId>
		void dismiss(PaletteNameInput&);

		template<ControlId>
		void confirmPositive(PaletteNameInput&);

		template<ControlId>
		void dismiss(ColorPicker&);

		template<ControlId>
		void confirmPositive(ColorPicker&);


	private:
		std::vector<Model::Palette> m_palettes;
		Model::ColorIndex m_sel_color_index;
		Model::ColorIndex m_modified_pal_index;

		using EventHandlerFunc = void (*)(void*, PaletteEditor&);
		void* r_eh;
		EventHandlerFunc r_func;

		void notify()
		{
			if(r_eh != nullptr) { r_func(r_eh, *this); }
		}

		Ui::Box m_container;
		Ui::Combobox m_pal_selector;
		Ui::PaletteView m_pal_view;
		std::unordered_set<std::string> m_used_pal_names;
		std::unique_ptr<PaletteNameInput> m_pal_name_input;
		std::array<Model::Pixel, 8> m_color_history;
		pcg64 m_rng;  // TODO:Should be global
		std::unique_ptr<ColorPicker> m_color_picker;
	};

	template<>
	inline void PaletteEditor::onChanged<PaletteEditor::ControlId::PalSelector>(Ui::Combobox& box)
	{
		m_pal_view.palette(m_palettes[m_pal_selector.selected()])
		    .highlightMode(Model::ColorIndex{0}, Texpainter::Ui::PaletteView::HighlightMode::Read)
		    .update();
		m_sel_color_index = Model::ColorIndex{0};
		notify();
	}

	template<>
	inline void PaletteEditor::onMouseUp<PaletteEditor::ControlId::PalView>(
	    Texpainter::Ui::PaletteView& view, Model::ColorIndex index, int button)
	{
		if(std::size(m_palettes) == 0)
		{
			createEmptyPalette();
			return;
		}

		if(index <= selectedPalette().lastIndex())
		{
			switch(button)
			{
				case 3:
				{
					m_modified_pal_index = index;
					m_pal_view
					    .highlightMode(index, Texpainter::Ui::PaletteView::HighlightMode::Write)
					    .update();
					m_color_picker = std::make_unique<ColorPicker>(
					    m_container,
					    (std::string{"Select color number "} + std::to_string(index.value() + 1))
					        .c_str(),
					    PolymorphicRng{m_rng},
					    "Recently used: ",
					    m_color_history);
					m_color_picker->eventHandler<ControlId::ColorPicker>(*this).widget().value(
					    selectedPalette()[index]);
				}
				break;

				case 1:
					m_pal_view
					    .highlightMode(m_sel_color_index,
					                   Texpainter::Ui::PaletteView::HighlightMode::None)
					    .highlightMode(index, Texpainter::Ui::PaletteView::HighlightMode::Read)
					    .update();
					m_sel_color_index = index;
					notify();
					break;
			}
		}
	}

	template<>
	inline void PaletteEditor::dismiss<PaletteEditor::ControlId::PalCreateEmpty>(PaletteNameInput&)
	{
		m_pal_name_input.reset();
	}

	template<>
	inline void PaletteEditor::confirmPositive<PaletteEditor::ControlId::PalCreateEmpty>(
	    PaletteNameInput& input)
	{
		createEmptyPalette(input.widget().inputField().content());
		m_pal_name_input.reset();
		notify();
	}

	template<>
	inline void PaletteEditor::dismiss<PaletteEditor::ControlId::ColorPicker>(ColorPicker&)
	{
		m_pal_view
		    .highlightMode(m_modified_pal_index, Texpainter::Ui::PaletteView::HighlightMode::None)
		    .highlightMode(m_sel_color_index, Texpainter::Ui::PaletteView::HighlightMode::Read);
		m_color_picker.reset();
	}

	template<>
	inline void PaletteEditor::confirmPositive<PaletteEditor::ControlId::ColorPicker>(
	    ColorPicker& picker)
	{
		auto color_new                                              = picker.widget().value();
		m_palettes[m_pal_selector.selected()][m_modified_pal_index] = color_new;

		m_pal_view.palette(m_palettes[m_pal_selector.selected()])
		    .highlightMode(m_modified_pal_index, Texpainter::Ui::PaletteView::HighlightMode::None)
		    .highlightMode(m_sel_color_index, Texpainter::Ui::PaletteView::HighlightMode::Read)
		    .update();

		std::rotate(std::rbegin(m_color_history),
		            std::rbegin(m_color_history) + 1,
		            std::rend(m_color_history));
		m_color_history[0] = color_new;
		m_color_picker.reset();
		notify();
	}
}

#endif