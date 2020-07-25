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
			msg += " Position: ";
			msg += toString(layer.location());
			msg += " Scale: ";
			msg += toString(layer.scaleFactor());
			msg += " Rotation: ";
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
void Texpainter::AppWindow::grabInit(vec2_t mouse_loc, Model::Layer const& current_layer)
{
	m_grab_state = m_trans_mode == TransformationMode::Absolute
	                   ? GrabState{vec2_t{0.0, 0.0}, vec2_t{0.0, 0.0}}
	                   : GrabState{current_layer.location(), mouse_loc};
}

void Texpainter::AppWindow::grab(vec2_t loc_current)
{
	auto layer = currentLayer(*m_current_document);
	if(layer == nullptr) [[unlikely]]
		{
			return;
		}

	auto const 𝒙  = m_grab_state.location(loc_current);
	auto const Δ𝒙 = 𝒙 - m_grab_state.initLocation();
	std::string info;
	info += "Grab Δ𝒙 = ";
	info += toString(Δ𝒙);
	m_paint_info.content(info.c_str());

	m_current_document->layersModify(
	    [loc = 𝒙, &current_layer = m_current_document->currentLayer()](auto& layers) {
		    if(auto layer = layers[current_layer]; layer != nullptr) [[likely]]
			    {
				    layer->location(loc);
			    }
		    return true;
	    });
	updateLayerInfo();
	doRender();
}

void Texpainter::AppWindow::scaleInit(vec2_t mouse_loc, Model::Layer const& current_layer)
{
	auto const ϴ     = current_layer.rotation();
	auto const rot_x = vec2_t{cos(ϴ), sin(ϴ)};
	auto const rot_y = vec2_t{-sin(ϴ), cos(ϴ)};
	auto const 𝒙     = transform(mouse_loc - current_layer.location(), rot_x, rot_y);

	m_scale_state = m_trans_mode == TransformationMode::Absolute
	                    ? ScaleState{vec2_t{1.0, 1.0}, 𝒙}
	                    : ScaleState{current_layer.scaleFactor(), 𝒙};
}

#if 0
namespace
{
	constexpr Texpainter::vec2_t snap_factors(Texpainter::vec2_t v)
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
};
#endif

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
	auto const 𝒙     = transform(loc_current - layer->location(), rot_x, rot_y);

	auto const 𝐬  = m_scale_state.scaleFactor(𝒙);
	auto const Ϙ𝐬 = 𝐬 / m_scale_state.initScale();
	std::string info;
	info += "Grab Ϙ𝐬 = ";
	info += toString(Ϙ𝐬);
	m_paint_info.content(info.c_str());

	m_current_document->layersModify(
	    [factor = 𝐬, &current_layer = m_current_document->currentLayer()](auto& layers) {
		    if(auto layer = layers[current_layer]; layer != nullptr) [[likely]]
			    {
				    layer->scaleFactor(factor);
			    }
		    return true;
	    });
	updateLayerInfo();
	doRender();
}


void Texpainter::AppWindow::rotateInit(vec2_t mouse_loc, Model::Layer const& current_layer)
{
	m_rot_state =
	    m_trans_mode == TransformationMode::Absolute
	        ? RotateState{Angle{0, Angle::Turns{}}, Angle{mouse_loc - current_layer.location()}}
	        : RotateState{current_layer.rotation(), Angle{mouse_loc - current_layer.location()}};
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
	constexpr auto CtrlLeftMask     = 0x1;
	constexpr auto ShiftLeftMask    = 0x1;
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

	auto snapfun =
	    (m_mod_state & CtrlLeftMask) ? [](Angle ϴ) { return snap(ϴ); } : [](Angle ϴ) { return ϴ; };

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

void Texpainter::AppWindow::printIdleInfo()
{
	std::string infostring{"LMB = "};
	switch(m_key_state.lastKey().value())
	{
		case GrabKey.value(): infostring += "grab"; break;

		case ScaleKey.value():
			infostring += "scale | LMB + 'G' + {Shift = Fix aspect ratio | Ctrl = Fixed ratios}";
			break;

		case RotateKey.value(): infostring += "rotate | LMB + 'R' + {Ctrl = Fixed angles}"; break;

		default:
			infostring += "LMB = paint | LMB + {'G' = grab (or move) | 'R' = rotate | 'S' = scale}";
	}
	infostring += ". ";
	if(m_trans_mode == TransformationMode::Absolute)
	{ infostring += "Transformations are relative to the canvas"; }
	else
	{
		infostring += "'A' enables canvas-relative transformations";
	}
	infostring += ".";
	m_paint_info.content(infostring.c_str());
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

	if(key == GrabKey || key == RotateKey || key == ScaleKey) { m_key_state.press(key); }

	switch(key.value())
	{
		case AbsTransformKey.value(): m_trans_mode = TransformationMode::Absolute; break;
		case CtrlLeft.value(): m_mod_state |= CtrlLeftMask; break;
		case ShiftLeft.value(): m_mod_state |= ShiftLeftMask; break;
	}

	if(m_layer_selector.inputField().selected() != -1) { printIdleInfo(); };
}

void Texpainter::AppWindow::onKeyUp(Ui::Scancode key)
{
	m_key_state.release(key);  // No operation if key not in set

	switch(key.value())
	{
		case AbsTransformKey.value(): m_trans_mode = TransformationMode::Relative; break;
		case CtrlLeft.value(): m_mod_state &= (~CtrlLeftMask); break;
		case ShiftLeft.value(): m_mod_state &= (~ShiftLeftMask); break;
	}
	m_key_prev = Ui::Scancode{0xff};

	if(m_layer_selector.inputField().selected() != -1) { printIdleInfo(); }
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
	printIdleInfo();
}

template<>
void Texpainter::AppWindow::onMouseMove<Texpainter::AppWindow::ControlId::Canvas>(
    Ui::ImageView& view, vec2_t loc_window, vec2_t loc_screen)
{
	if(auto layer = currentLayer(*m_current_document); layer != nullptr)
	{
		auto loc = ::toLogicalCoordinates(view.imageSize(), loc_window);
		if(m_mouse_state & MouseButtonLeft)
		{
			switch(m_key_state.lastKey().value())
			{
				case GrabKey.value():
					grab(loc);
					scaleInit(loc, *layer);
					rotateInit(loc, *layer);
					break;

				case ScaleKey.value():
					scale(loc);
					grabInit(loc, *layer);
					rotateInit(loc, *layer);
					break;

				case RotateKey.value():
					rotate(loc);
					grabInit(loc, *layer);
					scaleInit(loc, *layer);
					break;

				default:
					paint(loc);
					grabInit(loc, *layer);
					scaleInit(loc, *layer);
					rotateInit(loc, *layer);
			}
		}
		else
		{
			printIdleInfo();
			grabInit(loc, *layer);
			scaleInit(loc, *layer);
			rotateInit(loc, *layer);
		}
	}
}