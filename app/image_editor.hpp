//@	{"targets":[{"name":"image_editor.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_IMAGEEDITOR_HPP
#define TEXPAINTER_APP_IMAGEEDITOR_HPP

#include "./image_selector.hpp"
#include "./brush_selector.hpp"
#include "./palette_selector.hpp"

#include "model/document.hpp"
#include "model/paint.hpp"

#include "ui/image_view.hpp"
#include "ui/labeled_input.hpp"
#include "ui/separator.hpp"
#include "ui/color_picker.hpp"
#include "ui/dialog.hpp"
#include "ui/keyboard_state.hpp"
#include "ui/context.hpp"

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

			if(scancode.value() == 43) { return PixelStore::ColorIndex{15}; }

			return std::optional<PixelStore::ColorIndex>{};
		}

		inline std::optional<Model::BrushShape> toBrushShape(Ui::Scancode scancode)
		{
			switch(scancode.value())
			{
				case Ui::Scancodes::F1.value(): return Model::BrushShape::Diamond;
				case Ui::Scancodes::F2.value(): return Model::BrushShape::Circle;
				case Ui::Scancodes::F3.value(): return Model::BrushShape::Square;
				default: return std::optional<Model::BrushShape>{};
			}
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

		enum class DrawMode : int
		{
			Off,
			Paint,
			Erase
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
		    , m_loc{0.0, 0.0}
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
			if(auto i = imgs.find(current_img); i != std::end(imgs))
			{
				auto imgdisp = i->second.source.get();
				Model::drawOutline(imgdisp.pixels());
				m_img_view.image(imgdisp.pixels());
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
			updateBrush();
			return *this;
		}

		ImageEditor& refreshBrushSelector()
		{
			m_brush_sel.inputField().brush(m_doc.get().currentBrush());
			updateBrush();
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
				if(isShiftPressed(state)) { openColorPicker(*color_index); }
				else
				{
					selectColor(*color_index);
				}
				return;
			}

			if(auto brush_shape = detail::toBrushShape(scancode); brush_shape.has_value())
			{ selectBrush(*brush_shape); }
		}

		void selectColor(PixelStore::ColorIndex index)
		{
			auto const current_color = m_doc.get().currentColor();
			m_doc.get().currentColor(index);
			m_pal_sel.inputField()
			    .highlightMode(current_color, Ui::PaletteView::HighlightMode::None)
			    .highlightMode(index, Ui::PaletteView::HighlightMode::Read)
			    .update();
			updateBrush();
		}

		void selectBrush(Model::BrushShape shape)
		{
			auto brush = m_doc.get().currentBrush();
			m_doc.get().currentBrush(brush.shape(shape));
			refreshBrushSelector();
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
		void onMouseDown(Ui::ImageView& src, vec2_t loc_window, vec2_t, int button)
		{
			switch(button)
			{
				case 1:
					doPaint(loc_window);
					src.overlayLocation(loc_window);
					break;

				case 3:
					erase(loc_window);
					src.overlayLocation(loc_window);
					break;

				case 8: imgviewButtonBack(); break;

				case 9: imgviewButtonFwd(); break;
			}
		}

		template<auto>
		void onMouseUp(Ui::ImageView&, vec2_t, vec2_t, int)
		{
			m_draw_mode = DrawMode::Off;
		}

		template<auto>
		void onMouseMove(Ui::ImageView& src, vec2_t loc_window, vec2_t)
		{
			m_loc = loc_window;
			switch(m_draw_mode)
			{
				case DrawMode::Off: break;
				case DrawMode::Paint:
					paint(m_doc, loc_window);
					on_updated(r_eh, *this);
					break;

				case DrawMode::Erase:
				{
					auto brush      = m_doc.get().currentBrush();
					auto radius_new = std::clamp(brush.radius() + 1.0f / 32.0f, 0.0f, 1.0f);
					paint(m_doc, loc_window, radius_new, PixelStore::Pixel{0.0f, 0.0f, 0.0f, 0.0f});
					on_updated(r_eh, *this);
					break;
				}
			}
			src.overlayLocation(loc_window);
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
			updateBrush();
			m_img_view.overlayLocation(m_loc);
		}


	private:
		std::reference_wrapper<Model::Document> m_doc;
		DrawMode m_draw_mode;
		vec2_t m_loc;

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
				    PolymorphicRng{std::ref(DefaultRng::engine())},
				    "Recently used: ",
				    m_doc.get().colorHistory());
				m_color_picker->eventHandler<ImageEditor::ControlId::ColorPicker>(*this)
				    .widget()
				    .value(current_pal->source.get()[index]);
			}
		}

		void updateBrush()
		{
			auto const palette_ref = m_doc.get().palette(m_doc.get().currentPalette());
			auto const img_ref     = m_doc.get().image(m_doc.get().currentImage());
			if(palette_ref == nullptr || img_ref == nullptr)
			{
				m_img_view.overlay(Span2d<PixelStore::Pixel const>{});
				return;
			}

			auto const color    = palette_ref->source.get()[m_doc.get().currentColor()];
			auto const brush    = m_doc.get().currentBrush();
			auto const& imgdisp = img_ref->source.get();
			auto brush_radius   = ScalingFactors::sizeFromGeomMean(imgdisp.size(), brush.radius());
			PixelStore::Image rendered_brush{Size2d{static_cast<uint32_t>(brush_radius + 0.5),
			                                        static_cast<uint32_t>(brush_radius + 0.5)}};
			Model::paint(rendered_brush.pixels(),
			             0.5 * vec2_t{brush_radius, brush_radius},
			             0.5 * brush_radius,
			             Model::BrushFunction{brush.shape()},
			             color);
			m_img_view.overlay(rendered_brush);
		}

		template<class Selector>
		void goBack(Selector& sel)
		{
			using Type            = typename Selector::type;
			auto const& item_name = m_doc.get().current<Type>();
			if(auto item = m_doc.get().getBefore(std::type_identity<Type>{}, item_name);
			   item.first != nullptr)
			{
				sel.selected(*item.first, m_doc.get().get(std::type_identity<Type>{})).update();
				m_doc.get().current<Type>(Model::ItemName{*item.first});
			}
		}

		template<class Selector>
		void goForward(Selector& sel)
		{
			using Type            = typename Selector::type;
			auto const& item_name = m_doc.get().current<Type>();
			if(auto item = m_doc.get().getAfter(std::type_identity<Type>{}, item_name);
			   item.first != nullptr)
			{
				sel.selected(*item.first, m_doc.get().get(std::type_identity<Type>{})).update();
				m_doc.get().current<Type>(Model::ItemName{*item.first});
			}
		}

		void imgviewButtonBack()
		{
			auto& keyb_state = Ui::Context::get().keyboardState();
			if(isShiftPressed(keyb_state)) { goBack(m_image_sel.inputField()); }
			else
			{
				goBack(m_pal_sel.inputField());
			}
			on_updated(r_eh, *this);
		}

		void imgviewButtonFwd()
		{
			auto& keyb_state = Ui::Context::get().keyboardState();
			if(isShiftPressed(keyb_state)) { goForward(m_image_sel.inputField()); }
			else
			{
				goForward(m_pal_sel.inputField());
			}
			on_updated(r_eh, *this);
		}

		void erase(vec2_t loc_window)
		{
			auto& keyb_state = Ui::Context::get().keyboardState();
			if(isShiftPressed(keyb_state))
			{ floodfill(m_doc, loc_window, PixelStore::Pixel{0.0f, 0.0f, 0.0f, 0.0f}); }
			else
			{
				m_draw_mode     = DrawMode::Erase;
				auto brush      = m_doc.get().currentBrush();
				auto radius_new = std::clamp(brush.radius() + 1.0f / 32.0f, 0.0f, 1.0f);
				paint(m_doc, loc_window, radius_new, PixelStore::Pixel{0.0f, 0.0f, 0.0f, 0.0f});
			}
			on_updated(r_eh, *this);
		}

		void doPaint(vec2_t loc_window)
		{
			auto& keyb_state = Ui::Context::get().keyboardState();
			if(isShiftPressed(keyb_state)) { floodfill(m_doc, loc_window); }
			else
			{
				m_draw_mode = DrawMode::Paint;
				paint(m_doc, loc_window);
			}
			on_updated(r_eh, *this);
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
		updateBrush();
	}

	template<>
	inline void ImageEditor::onChanged<ImageEditor::ControlId::BrushSelector>(Ui::Slider& src)
	{
		auto brush = m_doc.get().currentBrush();
		m_doc.get().currentBrush(brush.radius(static_cast<float>(src.value().value())));
		updateBrush();
	}
}

#endif