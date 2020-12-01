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
		enum class ControlId : int
		{
			ImageSelector,
			PaletteSelector
		};

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
			refresh();
			m_image_sel.inputField().eventHandler<ControlId::ImageSelector>(*this);
			m_pal_sel.inputField().eventHandler<ControlId::PaletteSelector>(*this);
		}

		~ImageEditor() { m_doc.get().currentBrush(m_brush_sel.inputField().brush()); }

		ImageEditor& refresh()
		{
			auto& imgs = m_doc.get().images();
			if(std::size(imgs) == 0) [[unlikely]]
				{
					return *this;
				}

			m_image_sel.inputField()
			    .clear()
			    .appendFrom(std::ranges::transform_view(
			        imgs, [](auto const& item) { return item.first.c_str(); }))
			    .selected(m_doc.get().currentImage().c_str());

			auto i = imgs.find(m_doc.get().currentImage());
			if(i != std::end(imgs)) [[likely]]
				{
					m_img_view.image(i->second.source.get());
				}

			m_brush_sel.inputField().brush(m_doc.get().currentBrush());

			return *this;
		}

		template<auto>
		void onChanged(Ui::Combobox& src);

		template<auto>
		void onMouseDown(Ui::PaletteView& src, PixelStore::ColorIndex index, int button);

		template<auto, class... T>
		void onMouseUp(T&&...)
		{
		}

		template<auto, class... T>
		void onMouseMove(T&&...)
		{
		}

		template<auto, class T>
		void handleException(char const*, T&)
		{
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

	template<>
	inline void ImageEditor::onChanged<ImageEditor::ControlId::ImageSelector>(Ui::Combobox& src)
	{
		m_doc.get().currentImage(Model::ItemName{src.selected<char const*>()});
		refresh();
	}

	template<>
	inline void ImageEditor::onChanged<ImageEditor::ControlId::PaletteSelector>(Ui::Combobox& src)
	{
		m_doc.get().currentPalette(Model::ItemName{src.selected<char const*>()});
		refresh();
	}

	template<>
	inline void ImageEditor::onMouseDown<ImageEditor::ControlId::PaletteSelector>(
	    Ui::PaletteView& src, PixelStore::ColorIndex index, int button)
	{
		switch(button)
		{
			case 1:
			{
				auto const current_color = m_doc.get().currentColor();
				m_doc.get().currentColor(index);
				src.highlightMode(current_color, Ui::PaletteView::HighlightMode::None)
				    .highlightMode(index, Ui::PaletteView::HighlightMode::Read)
				    .update();
				break;
			}
			case 3: break;
		}
	}
}

#endif