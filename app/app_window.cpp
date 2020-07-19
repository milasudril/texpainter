//@	{"targets":[{"name":"app_window.o","type":"object"}]}

#include "./app_window.hpp"
#if 0
namespace
{
	constexpr Texpainter::Angle snap_angle(Texpainter::Angle ϴ)
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
   Ui::ImageView& view, vec2_t pos_window, vec2_t pos_screen)
{
	if(!m_painting) { return; }

	auto const size = view.imageSize();
	auto const offset =
	   0.5 * vec2_t{static_cast<double>(size.width()), static_cast<double>(size.height())};
	auto const loc = pos_window - offset;

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

		case PaintMode::Rotate:
		{
			if(m_keymask & KeymaskCtrl) { m_layerstack_ctrl.rotateCurrentLayer(loc, snap_angle); }
			else
			{
				m_layerstack_ctrl.rotateCurrentLayer(loc, unity<Angle>);
			}
			doRender();
		}
	}
}
#endif