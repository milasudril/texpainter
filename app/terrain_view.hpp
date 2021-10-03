//@	{
//@	 "targets":
//@		[{
//@		"name":"terrain_view.hpp",
//@		"type":"include",
//@		"pkgconfig_libs":["glew"],
//@		"dependencies": [{"ref": "GL", "rel":"external"}]
//@		}]
//@	,"dependencies_extra":[{"ref":"./terrain_view.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_APP_TERRAINVIEW_HPP
#define TEXPAINTER_APP_TERRAINVIEW_HPP

#include "model/document.hpp"

#include "ui/image_view.hpp"
#include "ui/glarea.hpp"
#include "ui/keyboard_state.hpp"
#include "ui/widget_multiplexer.hpp"

#include <GL/glew.h>
#include <utility>
#include <functional>

namespace Texpainter::App
{
	namespace terrain_view_detail
	{
		class GlResource
		{
		public:
			explicit GlResource(GLuint handle):m_handle{handle}{}

			GlResource(nullptr_t):m_handle{0}{}

			GlResource() = default;

			GlResource(GlResource const&) = default;

			GlResource& operator=(GlResource const&) = default;

			GlResource& operator=(std::nullptr_t)
			{
				m_handle = 0;
				return *this;
			}

			GLuint operator*() const
			{ return m_handle; }

			explicit operator bool() const
			{ return m_handle != 0; }

			bool operator==(GlResource const&) const = default;

		private:
			GLuint m_handle;
		};

		struct GlResourceTraits
		{
			using pointer = GlResource;
		};

		struct VertexBufferDeleter:GlResourceTraits
		{
			void operator()(GlResource id)
			{
				auto handle = *id;
				glDeleteBuffers(1, &handle);
			}
		};
	}

	using VertexBuffer = std::unique_ptr<GLuint, terrain_view_detail::VertexBufferDeleter>;

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
		void realize(Ui::GLArea const&);

		template<ControlId>
		void resize(Ui::GLArea const&, int, int)
		{
			puts("Resize");
		}

		TerrainView& canvasSize(Size2d size);

		TerrainView& topography(Span2d<Model::TopographyInfo const>);


	private:
		Ui::GLArea m_gl_area;
		VertexBuffer m_mesh;
		VertexBuffer m_topo;
	};
}

#endif