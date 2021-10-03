//@	{"targets":[{"name":"terrain_view.hpp","type":"include", "dependencies":[{"ref": "GL", "rel":"external"}]}]}

#ifndef TEXPAINTER_APP_TERRAINVIEW_HPP
#define TEXPAINTER_APP_TERRAINVIEW_HPP

#include "./terrain_view.hpp"

#include "model/document.hpp"

#include "ui/image_view.hpp"
#include "ui/glarea.hpp"
#include "ui/keyboard_state.hpp"
#include "ui/widget_multiplexer.hpp"

#include <GL/gl.h>
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

		explicit TerrainView(Ui::Container& owner): m_gl_area{owner}
		{
			m_gl_area.eventHandler<ControlId::GlArea>(*this);
		}

		template<ControlId>
		void render(Ui::GLArea const&)
		{
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			puts("Render");
		}

		template<ControlId>
		void realize(Ui::GLArea const&)
		{
			puts("Realize");
		}

		template<ControlId>
		void resize(Ui::GLArea const&, int, int)
		{
			puts("Resize");
		}

		TerrainView& canvasSize(Size2d size);

		TerrainView& topography(Span2d<Model::TopographyInfo const>);


	private:
		Ui::GLArea m_gl_area;
	};
}

#endif