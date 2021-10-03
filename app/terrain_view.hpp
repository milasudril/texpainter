//@	{"targets":[{"name":"terrain_view.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_TERRAINVIEW_HPP
#define TEXPAINTER_APP_TERRAINVIEW_HPP

#include "./terrain_view.hpp"

#include "model/document.hpp"

#include "ui/image_view.hpp"
#include "ui/glarea.hpp"
#include "ui/keyboard_state.hpp"
#include "ui/widget_multiplexer.hpp"

#include <utility>
#include <functional>

namespace Texpainter::App
{
	class TerrainView
	{
		enum class ControlId : int
		{
			GlArea
		};

	public:
		TerrainView(TerrainView&&) = delete;

		explicit TerrainView(Ui::Container& owner): m_gl_area{owner} {}

	private:
		Ui::GLArea m_gl_area;
	};
}

#endif