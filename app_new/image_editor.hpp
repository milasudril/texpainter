//@	{"targets":[{"name":"image_editor.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_IMAGEEDITOR_HPP
#define TEXPAINTER_APP_IMAGEEDITOR_HPP

#include "./image_selector.hpp"
#include "./brush_selector.hpp"
#include "./palette_selector.hpp"

#include "model_new/document.hpp"

#include "ui/image_view.hpp"
#include "ui/labeled_input.hpp"
#include "ui/separator.hpp"

#include <utility>

namespace Texpainter::App
{
	class ImageEditor
	{
	public:
		explicit ImageEditor(Ui::Container& owner, Model::Document& doc)
		    : m_doc{doc}
		    , m_root{owner, Ui::Box::Orientation::Vertical}
		    , m_selectors{m_root, Ui::Box::Orientation::Horizontal}
		    , m_image_sel{m_selectors, Ui::Box::Orientation::Horizontal, "Image: "}
		    , m_sep_a{m_selectors}
		    , m_brush_sel{m_selectors, Ui::Box::Orientation::Horizontal, "Brush: "}
		    , m_sep_b{m_selectors}
		    , m_pal_sel{m_selectors.insertMode(
		                    Ui::Box::InsertMode{0, Ui::Box::Fill | Ui::Box::Expand}),
		                Ui::Box::Orientation::Horizontal,
		                "Palette: "}
		    , m_img_view{m_root.insertMode(Ui::Box::InsertMode{0, Ui::Box::Fill | Ui::Box::Expand})}
		{
			m_brush_sel.inputField().brush(m_doc.get().currentBrush());
		}

	private:
		std::reference_wrapper<Model::Document> m_doc;
		Ui::Box m_root;
		Ui::Box m_selectors;
		Ui::LabeledInput<ImageSelector> m_image_sel;
		Ui::Separator m_sep_a;
		Ui::LabeledInput<BrushSelector> m_brush_sel;
		Ui::Separator m_sep_b;
		Ui::LabeledInput<PaletteSelector> m_pal_sel;
		Ui::ImageView m_img_view;
	};
}

#endif