//@	{"targets":[{"name":"image_editor.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_IMAGEEDITOR_HPP
#define TEXPAINTER_APP_IMAGEEDITOR_HPP

#include "./image_selector.hpp"
#include "./brush_selector.hpp"
#include "./palette_selector.hpp"

#include "model/document.hpp"

#include "ui/image_view.hpp"
#include "ui/labeled_input.hpp"
#include "ui/separator.hpp"
#include "ui/color_picker.hpp"
#include "ui/dialog.hpp"
#include "ui/keyboard_state.hpp"

#include "utils/default_rng.hpp"
#include "utils/inherit_from.hpp"

#include <utility>

namespace Texpainter::App
{
	namespace detail
	{
		inline std::optional<PixelStore::ColorIndex> toColorIndex(Ui::Scancode scancode)
		{
			if(scancode.value() >= 3 && scancode.value() < 8)
			{ return PixelStore::ColorIndex{static_cast<uint8_t>(scancode.value() - 3)}; }

			if(scancode.value() >= 16 && scancode.value() < 21)
			{ return PixelStore::ColorIndex{static_cast<uint8_t>(5 + scancode.value() - 16)}; }

			if(scancode.value() >= 30 && scancode.value() < 36)
			{ return PixelStore::ColorIndex{static_cast<uint8_t>(10 + scancode.value() - 30)}; }

			if(scancode.value() == 47) { return PixelStore::ColorIndex{15}; }

			return std::optional<PixelStore::ColorIndex>{};
		}
	}

	class ImageEditor
	{
		enum class ControlId : int
		{
			ImageSelector,
			BrushSelector,
			PaletteSelector,
			ColorPicker,
			DrawingArea
		};

		class ColorPickerData
		{
		public:
			explicit ColorPickerData(PixelStore::ColorIndex current_index)
			    : m_current_index{current_index}
			{
			}

			PixelStore::ColorIndex currentIndex() const { return m_current_index; }

		private:
			PixelStore::ColorIndex m_current_index;
		};

		using ColorPickerDlg =
		    Texpainter::Ui::Dialog<InheritFrom<ColorPickerData, Ui::ColorPicker>>;

	public:
		ImageEditor(ImageEditor&&) = delete;

		explicit ImageEditor(Ui::Container& owner, Model::Document& doc)
		    : m_doc{doc}
		    , m_draw_mode{false}
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
			m_brush_sel.inputField().eventHandler<ControlId::BrushSelector>(*this);
			m_pal_sel.inputField().eventHandler<ControlId::PaletteSelector>(*this);
			m_img_view.eventHandler<ControlId::DrawingArea>(*this);
			eventHandler<0>(*this);
		}

		~ImageEditor() { m_doc.get().currentBrush(m_brush_sel.inputField().brush()); }

		ImageEditor& refreshImageSelector()
		{
			auto& imgs = m_doc.get().images();
			m_image_sel.inputField()
			    .clear()
			    .appendFrom(std::ranges::transform_view(
			        imgs, [](auto const& item) { return item.first.c_str(); }))
			    .selected(m_doc.get().currentImage().c_str());

			refreshImageView();
			return *this;
		}

		ImageEditor& refreshImageView()
		{
			auto& imgs        = m_doc.get().images();
			auto& current_img = m_doc.get().currentImage();
			if(auto i = imgs.find(current_img); i != std::end(imgs)) [[likely]]
				{
					m_img_view.image(i->second.source.get().pixels());
				}
			else
			{
				m_img_view.image(Span2d<PixelStore::Pixel>{});
			}
			return *this;
		}

		ImageEditor& refreshPaletteSelector()
		{
			auto& pals        = m_doc.get().palettes();
			auto& current_pal = m_doc.get().currentPalette();
			m_pal_sel.inputField()
			    .clear()
			    .appendFrom(std::ranges::transform_view(
			        pals, [](auto const& item) { return item.first.c_str(); }))
			    .selected(current_pal, pals)
			    .highlightMode(m_doc.get().currentColor(), Ui::PaletteView::HighlightMode::Read);
			return *this;
		}

		ImageEditor& refreshBrushSelector()
		{
			m_brush_sel.inputField().brush(m_doc.get().currentBrush());
			return *this;
		}

		ImageEditor& refresh()
		{
			return refreshImageSelector().refreshBrushSelector().refreshPaletteSelector();
		}

		void onKeyDown(Ui::Scancode scancode, Ui::KeyboardState const& state)
		{
			if(auto color_index = detail::toColorIndex(scancode); color_index.has_value())
			{
				if(state.isPressed(Ui::Scancodes::ShiftLeft)
				   || state.isPressed(Ui::Scancodes::ShiftRight))
				{ openColorPicker(*color_index); }
				else
				{
					selectColor(*color_index);
				}
			}
		}

		void selectColor(PixelStore::ColorIndex index)
		{
			auto const current_color = m_doc.get().currentColor();
			m_doc.get().currentColor(index);
			m_pal_sel.inputField()
			    .highlightMode(current_color, Ui::PaletteView::HighlightMode::None)
			    .highlightMode(index, Ui::PaletteView::HighlightMode::Read)
			    .update();
		}

		template<auto>
		void onChanged(Ui::Combobox& src);

		template<auto>
		void onChanged(Ui::Slider& src);

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

		template<auto>
		void dismiss(ColorPickerDlg&)
		{
			refreshPaletteSelector();
			m_color_picker.reset();
		}

		template<auto>
		void confirmPositive(ColorPickerDlg& src)
		{
			m_doc.get().modify(
			    [index = src.widget().currentIndex(),
			     color = src.widget().value()](Model::Palette& pal) noexcept {
				    pal[index] = color;
				    return true;
			    },
			    m_doc.get().currentPalette());
			m_doc.get().currentColor(src.widget().currentIndex()).saveColor(src.widget().value());
			refreshPaletteSelector();
			on_updated(r_eh, *this);
			m_color_picker.reset();
		}

		template<auto>
		void onMouseDown(Ui::ImageView&, vec2_t loc_window, vec2_t, int button)
		{
			if(button == 1)
			{
				m_draw_mode = true;
				paint(m_doc, loc_window);
				on_updated(r_eh, *this);
			}
		}

		template<auto>
		void onMouseUp(Ui::ImageView&, vec2_t, vec2_t, int)
		{
			m_draw_mode = false;
		}

		template<auto>
		void onMouseMove(Ui::ImageView&, vec2_t loc_window, vec2_t)
		{
			if(m_draw_mode) [[likely]]
				{
					paint(m_doc, loc_window);
					on_updated(r_eh, *this);
				}
		}

		template<auto id, class EventHandler>
		ImageEditor& eventHandler(EventHandler& eh)
		{
			r_eh       = &eh;
			on_updated = [](void* self, ImageEditor& src) {
				reinterpret_cast<EventHandler*>(self)->template onUpdated<id>(src);
			};
			return *this;
		}

		template<auto>
		void onScroll(Ui::ImageView&, vec2_t delta)
		{
			auto brush = m_doc.get().currentBrush();
			auto radius_new =
			    std::clamp(brush.radius() + static_cast<float>(delta[1]) / 32.0f, 0.0f, 1.0f);
			brush.radius(radius_new);
			m_brush_sel.inputField().brush(brush);
			m_doc.get().currentBrush(brush);
		}


	private:
		std::reference_wrapper<Model::Document> m_doc;
		bool m_draw_mode;
		void* r_eh;
		void (*on_updated)(void*, ImageEditor&);

		template<auto>
		void onUpdated(ImageEditor& src)
		{
			src.refresh();
		}

		Ui::Box m_root;
		Ui::Box m_selectors;
		Ui::LabeledInput<ImageSelector> m_image_sel;
		Ui::Separator m_sep_a;
		Ui::LabeledInput<BrushSelector> m_brush_sel;
		Ui::Separator m_sep_b;
		Ui::LabeledInput<PaletteSelector> m_pal_sel;
		Ui::ImageView m_img_view;

		std::unique_ptr<ColorPickerDlg> m_color_picker;

		void openColorPicker(PixelStore::ColorIndex index)
		{
			if(auto current_pal = m_doc.get().palette(m_doc.get().currentPalette());
			   current_pal != nullptr)
			{
				auto& src = m_pal_sel.inputField();
				src.highlightMode(index, Texpainter::Ui::PaletteView::HighlightMode::Write)
				    .update();
				m_color_picker = std::make_unique<ColorPickerDlg>(
				    index,
				    m_selectors,
				    (std::string{"Select color number "} + std::to_string(index.value() + 1))
				        .c_str(),
				    PolymorphicRng{DefaultRng::engine()},
				    "Recently used: ",
				    m_doc.get().colorHistory());
				m_color_picker->eventHandler<ImageEditor::ControlId::ColorPicker>(*this)
				    .widget()
				    .value(current_pal->source.get()[index]);
			}
		}
	};

	template<>
	inline void ImageEditor::onChanged<ImageEditor::ControlId::ImageSelector>(Ui::Combobox& src)
	{
		m_doc.get().currentImage(Model::ItemName{src.selected<char const*>()});
		on_updated(r_eh, *this);
	}

	template<>
	inline void ImageEditor::onChanged<ImageEditor::ControlId::PaletteSelector>(Ui::Combobox& src)
	{
		m_doc.get().currentPalette(Model::ItemName{src.selected<char const*>()});
		on_updated(r_eh, *this);
	}

	template<>
	inline void ImageEditor::onMouseDown<ImageEditor::ControlId::PaletteSelector>(
	    Ui::PaletteView&, PixelStore::ColorIndex index, int button)
	{
		switch(button)
		{
			case 1:
			{
				selectColor(index);
				break;
			}

			case 3:
			{
				openColorPicker(index);
				break;
			}
		}
	}

	template<>
	inline void ImageEditor::onChanged<ImageEditor::ControlId::BrushSelector>(Ui::Combobox& src)
	{
		auto brush = m_doc.get().currentBrush();
		m_doc.get().currentBrush(brush.shape(static_cast<Model::BrushShape>(src.selected())));
	}

	template<>
	inline void ImageEditor::onChanged<ImageEditor::ControlId::BrushSelector>(Ui::Slider& src)
	{
		auto brush = m_doc.get().currentBrush();
		m_doc.get().currentBrush(brush.radius(static_cast<float>(src.value().value())));
	}
}

#endif