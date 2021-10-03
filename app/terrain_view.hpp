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
	class GlHandle
	{
	public:
		explicit GlHandle(GLuint handle) noexcept: m_handle{handle} {}

		GlHandle(nullptr_t) noexcept: m_handle{0} {}

		GlHandle() = default;

		GlHandle(GlHandle const&) = default;

		GlHandle& operator=(GlHandle const&) = default;

		GlHandle& operator=(std::nullptr_t) noexcept
		{
			m_handle = 0;
			return *this;
		}

		GLuint operator*() const noexcept { return m_handle; }

		explicit operator bool() const noexcept { return m_handle != 0; }

		bool operator==(GlHandle const&) const = default;

	private:
		GLuint m_handle;
	};

	namespace terrain_view_detail
	{
		struct GlResourceTraits
		{
			using pointer = GlHandle;
		};

		struct VertexBufferDeleter: GlResourceTraits
		{
			void operator()(GlHandle id)
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

		explicit TerrainView(Ui::Container& owner): m_gl_area{owner}, m_mesh_size{0, 0}
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

		TerrainView& meshSize(Size2d size);

		TerrainView& topography(Span2d<Model::TopographyInfo const>);

	private:
		Ui::GLArea m_gl_area;
		VertexBuffer m_xy;
		VertexBuffer m_topo;
		VertexBuffer m_faces;
		Size2d m_mesh_size;
	};
}

#endif