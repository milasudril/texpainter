//@	{"targets":[{"name":"app_window.o","type":"object"}]}

#include "./app_window.hpp"

Texpainter::AppWindow::AppWindow(Ui::Container& container, PolymorphicRng rng)
    :  // State
    m_rng{rng}
    , m_mouse_state{0}
    , m_key_prev{0xff}
    , m_trans_mode{TransformationMode::Relative}

    // Event handlers
    , m_doc_menu_handler{container, *this}
    , m_layer_menu_handler{container, *this, m_rng}
    , m_palette_menu_handler{container, *this, m_rng}
    , m_pal_view_eh{container, *this, m_rng}

    // Widgets
    , m_rows{container, Ui::Box::Orientation::Vertical}
    , m_menu{m_rows}
    , m_selectors{m_rows, Ui::Box::Orientation::Horizontal}
    , m_layer_selector{m_selectors, Ui::Box::Orientation::Horizontal, "Layer: "}
    , m_sep0{m_selectors.insertMode(Ui::Box::InsertMode{4, 0})}
    , m_brush_selector{m_selectors.insertMode(Ui::Box::InsertMode{0, 0}),
                       Ui::Box::Orientation::Horizontal,
                       "Brush: "}
    , m_brush_size{m_selectors, false}
    , m_sep1{m_selectors.insertMode(Ui::Box::InsertMode{4, 0})}
    , m_palette_selector{m_selectors.insertMode(Ui::Box::InsertMode{0, 0}),
                         Ui::Box::Orientation::Horizontal,
                         "Palette: "}
    , m_pal_view{m_selectors.insertMode(Ui::Box::InsertMode{4, Ui::Box::Fill | Ui::Box::Expand})}
    , m_info{m_rows, Ui::Box::Orientation::Horizontal}
    , m_layer_info{m_info, ""}
    , m_paint_info{m_info.insertMode(Ui::Box::InsertMode{4, Ui::Box::Fill | Ui::Box::Expand}), ""}
    , m_img_view{m_rows.insertMode(Ui::Box::InsertMode{0, Ui::Box::Fill | Ui::Box::Expand})}


{
	forEachEnumItem<Model::BrushType>([&brush_sel = m_brush_selector.inputField()](auto tag) {
		brush_sel.append(Model::BrushTraits<tag.value>::displayName());
	});
	m_layer_info.oneline(true)
	    .content("Open the \"Document\" menu to create or open a document")
	    .alignment(0.0f);
	m_paint_info.oneline(true).alignment(1.0f);
	m_menu.eventHandler(*this);
}

void Texpainter::AppWindow::updateLayerSelector()
{
	auto& layer_selector = m_layer_selector.inputField();

	layer_selector.clear();
	std::ranges::for_each(
	    m_current_document->layers().keysByIndex(),
	    [&layer_selector](auto const& name) { layer_selector.append(name.c_str()); });

	auto const& current_layer    = m_current_document->currentLayer();
	auto const current_layer_idx = m_current_document->layers().index(current_layer);
	layer_selector.selected(static_cast<int>(current_layer_idx));
}

void Texpainter::AppWindow::updateBrushSelector()
{
	auto brush = m_current_document->currentBrush();
	auto brush_index =
	    static_cast<int>(end(Empty<Model::BrushType>{})) - 1 - static_cast<int>(brush.type());
	m_brush_selector.inputField().selected(brush_index);
	m_brush_size.value(Ui::logValue(brush.radius()));
}

void Texpainter::AppWindow::updatePaletteSelector()
{
	auto& pal_selector = m_palette_selector.inputField();

	pal_selector.clear();
	std::ranges::for_each(m_current_document->palettes().keys(),
	                      [&pal_selector](auto const& name) { pal_selector.append(name.c_str()); });

	auto const& current_pal_name = m_current_document->currentPalette();
	auto const current_layer_idx = m_current_document->palettes().location(current_pal_name);
	pal_selector.selected(static_cast<int>(current_layer_idx));

	if(auto pal = currentPalette(*m_current_document); pal != nullptr)
	{
		m_pal_view.palette(*pal)
		    .highlightMode(m_current_document->currentColor(), Ui::PaletteView::HighlightMode::Read)
		    .update();
	}
	else
	{
		Model::Pixel color{0.0f, 0.0f, 0.0f, 0.0f};
		m_pal_view.palette(std::span{&color, 1}).update();
	}
}

void Texpainter::AppWindow::updateLayerInfo()
{
	if(auto current_layer = currentLayer(*m_current_document); current_layer != nullptr) [[likely]]
		{
			std::string layer_info;
			auto const& layer = *current_layer;
			std::string msg{"Layer "};
			msg += std::to_string(
			    m_current_document->layers().location(m_current_document->currentLayer()));
			msg += ". Size: ";
			msg += std::to_string(layer.size().width());
			msg += "×";
			msg += std::to_string(layer.size().height());
			msg += " Position: (";
			msg += std::to_string(layer.location()[0]);
			msg += ", ";
			msg += std::to_string(layer.location()[1]);
			msg += ") Scale: (";
			msg += std::to_string(layer.scaleFactor()[0]);
			msg += ", ";
			msg += std::to_string(layer.scaleFactor()[1]);
			msg += ") Rotation: ";
			msg += std::to_string(layer.rotation().turns());
			m_layer_info.content(msg.c_str());
		}
	else
	{
		m_layer_info.content("Open the \"Layer\" menu to create a new layer").alignment(0.0f);
	}
}


void Texpainter::AppWindow::update()
{
	updateLayerSelector();
	updateBrushSelector();
	updatePaletteSelector();
	updateLayerInfo();
	doRender();
}

void Texpainter::AppWindow::doRender()
{
	Model::Image canvas{m_current_document->canvasSize()};
	std::ranges::fill(canvas.pixels(), Model::Pixel{0.0f, 0.0f, 0.0f, 0.0f});
	std::ranges::for_each(m_current_document->layersByIndex(), [&canvas](auto const& layer) {
		if(layer.visible()) { render(layer, canvas); }
	});

	if(auto current_layer = currentLayer(*m_current_document); current_layer != nullptr) [[likely]]
		{
			outline(*current_layer, canvas);
		}
	m_img_view.image(canvas);
}

void Texpainter::AppWindow::paint(vec2_t loc)
{
	m_current_document->layersModify(
	    [loc,
	     radius         = m_current_document->currentBrush().radius(),
	     brush          = Model::BrushFunction{m_current_document->currentBrush().type()},
	     color          = currentColor(*m_current_document),
	     &current_layer = m_current_document->currentLayer()](auto& layers) {
		    if(auto layer = layers[current_layer]; layer != nullptr) [[likely]]
			    {
				    auto const scale = static_cast<float>(std::sqrt(layer->size().area()));
				    layer->paint(loc, scale * radius, brush, color);
			    }
		    return true;
	    });
	doRender();
}
void Texpainter::AppWindow::grabInit(vec2_t mouse_loc)
{
	if(auto layer = currentLayer(*m_current_document); layer != nullptr)
	{
		m_grab_state = m_trans_mode == TransformationMode::Absolute
		                   ? GrabState{vec2_t{0.0, 0.0}, vec2_t{0.0, 0.0}}
		                   : GrabState{layer->location(), mouse_loc};
	}
}

void Texpainter::AppWindow::grab(vec2_t loc_current)
{
	m_current_document->layersModify(
	    [loc            = m_grab_state.location(loc_current),
	     &current_layer = m_current_document->currentLayer()](auto& layers) {
		    if(auto layer = layers[current_layer]; layer != nullptr) [[likely]]
			    {
				    layer->location(loc);
			    }
		    return true;
	    });
	updateLayerInfo();
	doRender();
}

void Texpainter::AppWindow::scaleInit(vec2_t mouse_loc)
{
	if(auto layer = currentLayer(*m_current_document); layer != nullptr)
	{
		auto const ϴ     = layer->rotation();
		auto const rot_x = vec2_t{cos(ϴ), sin(ϴ)};
		auto const rot_y = vec2_t{-sin(ϴ), cos(ϴ)};
		auto const v     = transform(mouse_loc - layer->location(), rot_x, rot_y);

		m_scale_state = m_trans_mode == TransformationMode::Absolute
		                    ? ScaleState{vec2_t{1.0, 1.0}, v}
		                    : ScaleState{layer->scaleFactor(), v};
	}
}

void Texpainter::AppWindow::scale(vec2_t loc_current)
{
	auto layer = currentLayer(*m_current_document);
	if(layer == nullptr) [[unlikely]]
		{
			return;
		}

	auto const ϴ     = layer->rotation();
	auto const rot_x = vec2_t{cos(ϴ), sin(ϴ)};
	auto const rot_y = vec2_t{-sin(ϴ), cos(ϴ)};
	auto const v     = transform(loc_current - layer->location(), rot_x, rot_y);

	m_current_document->layersModify(
	    [factor         = m_scale_state.scaleFactor(v),
	     &current_layer = m_current_document->currentLayer()](auto& layers) {
		    if(auto layer = layers[current_layer]; layer != nullptr) [[likely]]
			    {
				    layer->scaleFactor(factor);
			    }
		    return true;
	    });
	updateLayerInfo();
	doRender();
}


void Texpainter::AppWindow::rotateInit(vec2_t mouse_loc)
{
	if(auto layer = currentLayer(*m_current_document); layer != nullptr)
	{
		m_rot_state =
		    m_trans_mode == TransformationMode::Absolute
		        ? RotateState{Angle{0, Angle::Turns{}}, Angle{mouse_loc - layer->location()}}
		        : RotateState{layer->rotation(), Angle{mouse_loc - layer->location()}};
	}
}

namespace
{
	constexpr auto MouseButtonLeft  = 0x1;
	constexpr auto MouseButtonRight = 0x4;
	constexpr auto AbsTransformKey  = Texpainter::Ui::Scancode{30};
	constexpr auto GrabKey          = Texpainter::Ui::Scancode{34};
	constexpr auto RotateKey        = Texpainter::Ui::Scancode{19};
	constexpr auto ScaleKey         = Texpainter::Ui::Scancode{31};
	constexpr auto CtrlLeft         = Texpainter::Ui::Scancode{29};
	constexpr auto ShiftLeft        = Texpainter::Ui::Scancode{42};
}

namespace
{
	constexpr Texpainter::Angle snap(Texpainter::Angle ϴ)
	{
		using Texpainter::Angle;
		constexpr Angle snap_angles[] = {
		    Angle{0.0 / 12, Angle::Turns{}},
		    Angle{1.0 / 12, Angle::Turns{}},
		    Angle{2.0 / 12, Angle::Turns{}},
		    Angle{3.0 / 12, Angle::Turns{}},
		    Angle{4.0 / 12, Angle::Turns{}},
		    Angle{5.0 / 12, Angle::Turns{}},
		    Angle{6.0 / 12, Angle::Turns{}},
		    Angle{7.0 / 12, Angle::Turns{}},
		    Angle{8.0 / 12, Angle::Turns{}},
		    Angle{9.0 / 12, Angle::Turns{}},
		    Angle{10.0 / 12, Angle::Turns{}},
		    Angle{11.0 / 12, Angle::Turns{}},
		    Angle{1.0 / 8, Angle::Turns{}},
		    Angle{3.0 / 8, Angle::Turns{}},
		    Angle{5.0 / 8, Angle::Turns{}},
		    Angle{7.0 / 8, Angle::Turns{}},
		    Angle{(1.0 - 1.0 / std::numbers::phi) / 2.0, Angle::Turns{}},
		    Angle{(1.0 - 1.0 / std::numbers::phi), Angle::Turns{}},
		    Angle{3.0 * (1.0 - 1.0 / std::numbers::phi) / 2.0, Angle::Turns{}},
		    Angle{4.0 * (1.0 - 1.0 / std::numbers::phi) / 2.0, Angle::Turns{}},
		    Angle{5.0 * (1.0 - 1.0 / std::numbers::phi) / 2.0, Angle::Turns{}}};


		constexpr auto snap = Texpainter::Snap{snap_angles};

		return snap.nearestValue(ϴ);
	}
}

void Texpainter::AppWindow::rotate(vec2_t loc_current)
{
	auto layer = currentLayer(*m_current_document);
	if(layer == nullptr) [[unlikely]]
		{
			return;
		}

	auto snapfun = m_mod_state.lastKey() == CtrlLeft ? [](Angle ϴ) { return snap(ϴ); }
	                                                 : [](Angle ϴ) { return ϴ; };

	auto const Θ  = m_rot_state.rotation(Angle{loc_current - layer->location()}, snapfun);
	auto const ΔΘ = Θ - m_rot_state.initRotation();
	std::string info;
	info += "Rotate ΔΘ = ";
	info += std::to_string(ΔΘ.turns());
	m_paint_info.content(info.c_str());

	m_current_document->layersModify(
	    [rot = Θ, &current_layer = m_current_document->currentLayer()](auto& layers) {
		    if(auto layer = layers[current_layer]; layer != nullptr) [[likely]]
			    {
				    layer->rotation(rot);
			    }
		    return true;
	    });
	updateLayerInfo();
	doRender();
}

void Texpainter::AppWindow::printIdleInfo(vec2_t mouse_loc)
{
	std::string info;
	info += "LMB = paint | LMB + {'G' = grab (or move) | 'R' = rotate | 'S' = scale}. ";
	if(m_trans_mode == TransformationMode::Absolute)
	{ info += " Transformations are relative to the canvas."; }
	else
	{
		info += "'A' enables canvas-relative transformations.";
	}
	m_paint_info.content(info.c_str());
}


namespace
{
	Texpainter::vec2_t toLogicalCoordinates(Texpainter::Size2d size, Texpainter::vec2_t location)
	{
		auto const offset = 0.5
		                    * Texpainter::vec2_t{static_cast<double>(size.width()),
		                                         static_cast<double>(size.height())};
		return location - offset;
	}
}


void Texpainter::AppWindow::onKeyDown(Ui::Scancode key)
{
	if(key == m_key_prev) { return; }
	m_key_prev = key;

	if(key == AbsTransformKey) { m_trans_mode = TransformationMode::Absolute; }

	if(key == GrabKey || key == RotateKey || key == ScaleKey) { m_key_state.press(key); }

	if(key == CtrlLeft || key == ShiftLeft) { m_mod_state.press(key); }
}

void Texpainter::AppWindow::onKeyUp(Ui::Scancode key)
{
	m_key_state.release(key);  // No operation if key not in set
	m_mod_state.release(key);

	switch(key.value())
	{
		case AbsTransformKey.value(): m_trans_mode = TransformationMode::Relative; break;
	}
	m_key_prev = Ui::Scancode{0xff};
}


template<>
void Texpainter::AppWindow::onMouseDown<Texpainter::AppWindow::ControlId::Canvas>(
    Ui::ImageView& view, vec2_t loc_window, vec2_t loc_screen, int button)
{
	m_mouse_state |= 1 << (button - 1);
	auto loc = ::toLogicalCoordinates(view.imageSize(), loc_window);

	if(m_mouse_state & MouseButtonLeft)
	{
		switch(m_key_state.lastKey().value())
		{
			case GrabKey.value(): grab(loc); break;

			case ScaleKey.value(): scale(loc); break;

			case RotateKey.value(): rotate(loc); break;

			default: paint(loc);
		}
	}
}

template<>
void Texpainter::AppWindow::onMouseUp<Texpainter::AppWindow::ControlId::Canvas>(Ui::ImageView& view,
                                                                                vec2_t loc_window,
                                                                                vec2_t loc_screen,
                                                                                int button)
{
	m_mouse_state &= ~(1 << (button - 1));
	m_paint_info.content("");
}

template<>
void Texpainter::AppWindow::onMouseMove<Texpainter::AppWindow::ControlId::Canvas>(
    Ui::ImageView& view, vec2_t loc_window, vec2_t loc_screen)
{
	// FIXME: This function should not do anything unless there is a current layer.
	auto loc = ::toLogicalCoordinates(view.imageSize(), loc_window);

	if(m_mouse_state & MouseButtonLeft)
	{
		switch(m_key_state.lastKey().value())
		{
			case GrabKey.value():
				grab(loc);
				scaleInit(loc);
				rotateInit(loc);
				break;

			case ScaleKey.value():
				scale(loc);
				grabInit(loc);
				rotateInit(loc);
				break;

			case RotateKey.value():
				rotate(loc);
				grabInit(loc);
				scaleInit(loc);
				break;

			default:
				paint(loc);
				grabInit(loc);
				scaleInit(loc);
				rotateInit(loc);
				printIdleInfo(loc);
		}
	}
	else
	{
		grabInit(loc);
		scaleInit(loc);
		rotateInit(loc);
		printIdleInfo(loc);
	}
}


#if 0
namespace
{


	constexpr Texpainter::vec2_t snap_scale_factor(Texpainter::vec2_t v)
	{
		constexpr double snap_factors[] = {-(1.0 / 8),
		                                   -(1.0 / 7),
		                                   -(1.0 / 6),
		                                   -(1.0 / 5),
		                                   -(1.0 / 4),
		                                   -(1.0 / 3),
		                                   -(1.0 / 2),
		                                   -(1.0),
		                                   -(2.0),
		                                   -(3.0),
		                                   -(4.0),
		                                   -(5.0),
		                                   -(7.0),
		                                   -(8.0),
		                                   -(1.0),
		                                   -(1.0 / std::numbers::phi),
		                                   -(std::numbers::phi),
		                                   -(1.0 - 1.0 / std::numbers::phi),
		                                   1.0 / 8,
		                                   1.0 / 7,
		                                   1.0 / 6,
		                                   1.0 / 5,
		                                   1.0 / 4,
		                                   1.0 / 3,
		                                   1.0 / 2,
		                                   1.0,
		                                   2.0,
		                                   3.0,
		                                   4.0,
		                                   5.0,
		                                   7.0,
		                                   8.0,
		                                   1.0,
		                                   1.0 / std::numbers::phi,
		                                   std::numbers::phi,
		                                   1.0 - 1.0 / std::numbers::phi};

		constexpr auto snap = Texpainter::Snap{snap_factors};

		return Texpainter::vec2_t{snap.nearestValue(v[0]), snap.nearestValue(v[1])};
	}

	constexpr Texpainter::vec2_t keep_aspect_ratio(Texpainter::vec2_t v)
	{
		auto const factor = Texpainter::length(v);

		return Texpainter::vec2_t{factor, factor};
	}

	template<class T>
	constexpr T unity(T val)
	{
		return val;
	}
};

template<>
void Texpainter::AppWindow::onMouseMove<Texpainter::AppWindow::ControlId::Canvas>(
   Ui::ImageView& view, vec2_t loc_window, vec2_t loc_screen)
{
	if(!m_painting) { return; }

	auto const size = view.imageSize();
	auto const offset =
	   0.5 * vec2_t{static_cast<double>(size.width()), static_cast<double>(size.height())};
	auto const loc = loc_window - offset;

	switch(m_paintmode)
	{
		case PaintMode::Draw:
		{
			m_layerstack_ctrl.paintCurrentLayer(loc, 4.0, m_current_color);
			doRender();
		}
		break;

		case PaintMode::Grab:
		{
			m_layerstack_ctrl.moveCurrentLayer(loc);
			doRender();
		}
		break;

		case PaintMode::Scale:
		{
			if(m_keymask & KeymaskCtrl)
			{ m_layerstack_ctrl.scaleCurrentLayer(loc, m_paint_start_pos, snap_scale_factor); }
			else if(m_keymask & KeymaskShift)
			{
				m_layerstack_ctrl.scaleCurrentLayer(loc, m_paint_start_pos, keep_aspect_ratio);
			}
			else
			{
				m_layerstack_ctrl.scaleCurrentLayer(loc, m_paint_start_pos, unity<vec2_t>);
			}
			doRender();
		}
		break;

	}
}
#endif