//	{
//	 "targets":[{"name":"layerstack_control.o", "type":"object"}]
//	}

#include "./layerstack_control.hpp"

namespace
{
	void draw_marker(Texpainter::vec2_t pos, Texpainter::Span2d<Texpainter::Model::Pixel> canvas)
	{
		constexpr auto radius   = 2;
		auto const w            = canvas.width();
		auto const h            = canvas.height();
		auto const begin_coords = pos - Texpainter::vec2_t{radius, radius};
		auto const end_coords   = pos + Texpainter::vec2_t{radius, radius};

		for(int row = static_cast<int>(begin_coords[1]); row <= static_cast<int>(end_coords[1]);
		    ++row)
		{
			for(int col = static_cast<int>(begin_coords[0]); col <= static_cast<int>(end_coords[0]);
			    ++col)
			{
				auto const loc_ret =
				    Texpainter::vec2_t{static_cast<double>(col), static_cast<double>(row)};
				auto d = loc_ret - pos;
				if(Texpainter::dot(d, d) < radius * radius)
				{
					auto& pixel = canvas(col % w, row % h);
					pixel       = Texpainter::Model::Pixel{1.0f, 1.0f, 1.0f, 1.0f} - pixel;
					pixel.alpha(1.0f);
				}
			}
		}
	}
}

Texpainter::LayerStackControl& Texpainter::LayerStackControl::outlineCurrentLayer(
    Layer const& layer, Span2d<Pixel> ret)
{
	if(m_current_layer.valid())
	{
		auto const w              = canvas.width();
		auto const h              = canvas.height();
		auto const offset         = 0.5 * vec2_t{static_cast<double>(w), static_cast<double>(h)};
		auto const& current_layer = m_layers[m_current_layer];

		auto const origin       = current_layer.location() + offset;
		auto const ϴ            = current_layer.rotation();
		auto const rot_x        = vec2_t{cos(ϴ), -sin(ϴ)};
		auto const rot_y        = vec2_t{sin(ϴ), cos(ϴ)};
		auto const scale_factor = current_layer.scaleFactor();

		auto const size_layer = current_layer.size();
		auto const box_layer  = 0.5 * scale_factor
		                       * vec2_t{static_cast<double>(size_layer.width()),
		                                static_cast<double>(size_layer.height())};
		draw_marker(origin, canvas);
		draw_marker(origin + transform(box_layer * vec2_t{1.0, 1.0}, rot_x, rot_y), canvas);
		draw_marker(origin + transform(box_layer * vec2_t{-1.0, 1.0}, rot_x, rot_y), canvas);
		draw_marker(origin + transform(box_layer * vec2_t{-1.0, -1.0}, rot_x, rot_y), canvas);
		draw_marker(origin + transform(box_layer * vec2_t{1.0, -1.0}, rot_x, rot_y), canvas);
	}
	return *this;
}
