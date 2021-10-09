//@	{
//@	 "targets":
//@		[{
//@		"name":"terrain_view.o",
//@		"type":"object"
//@		}]
//@	}

#include "./terrain_view.hpp"
#include "log/logger.hpp"

#define GLM_FORCE_INLINE
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace
{
	Texpainter::App::Shader make_shader(char const* src, GLuint type)
	{
		auto ret = Texpainter::App::Shader{Texpainter::App::GlHandle{glCreateShader(type)}};
		glShaderSource(*ret.get(), 1, &src, nullptr);
		glCompileShader(*ret.get());
		return ret;
	}

	template<size_t N>
	Texpainter::App::ShaderProgram make_program(std::array<Texpainter::App::Shader, N> shaders)
	{
		auto ret = Texpainter::App::ShaderProgram{Texpainter::App::GlHandle{glCreateProgram()}};

		for(size_t k = 0; k != N; ++k)
		{
			glAttachShader(*ret.get(), *shaders[k].get());
		}

		glLinkProgram(*ret.get());
		return ret;
	}

	constexpr char const* vertex_shader = R"shader(#version 460 core
layout (location = 0) in vec2 xy;
layout (location = 1) in vec4 n_elev;
layout (location = 2) uniform float scale;
layout (location = 3) uniform vec4 cam_loc;
layout (location = 7) uniform mat4 cam_rot;

out vec4 vertex_normal;
out vec4 frag_pos;

void main()
{
	vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
	vec4 vert_world_loc = vec4(xy.xy, n_elev.w, 1.0);
	vec4 vert_world_loc_scaled = (vert_world_loc - origin)/scale + origin;
	vec4 vert_cam_loc = cam_rot*(vert_world_loc_scaled - origin) + origin + (cam_loc - origin) + origin;

	gl_Position = vert_cam_loc;

	frag_pos = vert_world_loc_scaled;
	vertex_normal = vec4(n_elev.xyz, 0.0);
}
)shader";

	constexpr char const* frag_shader = R"shader(#version 460 core
layout(location = 0) out vec4 frag_color;

in vec4 vertex_normal;
in vec4 frag_pos;

void main()
{
	frag_color = vec4(frag_pos.x + 0.5, frag_pos.y + 0.5, frag_pos.z, 1.0);
}
)shader";
}

template<>
void Texpainter::App::TerrainView::realize<Texpainter::App::TerrainView::ControlId::GlArea>(
    Ui::GLArea const&)
{
	glewExperimental = true;
	if(auto err = glewInit(); err != GLEW_OK)
	{
		Logger::log(Logger::MessageType::Error,
		            reinterpret_cast<char const*>(glewGetErrorString(err)));
		return;
	}
	m_initialized = true;

	Logger::log(Logger::MessageType::Info, reinterpret_cast<char const*>(glGetString(GL_VERSION)));

	std::array<Shader, 2> shaders{{make_shader(vertex_shader, GL_VERTEX_SHADER),
	                               make_shader(frag_shader, GL_FRAGMENT_SHADER)}};

	m_shader_program = make_program(std::move(shaders));
	glUseProgram(*m_shader_program.get());

	auto const cam_loc = glm::vec4{0.0f, -1.0f, 0.0f * (std::numbers::phi_v<float> - 1.0f), 1.0f};
	glUniform4f(3, cam_loc.x, cam_loc.y, cam_loc.z, cam_loc.w);

	auto const cam_x   = 0.5f * std::numbers::pi_v<float>;
	auto const cam_y   = -1.0f * std::numbers::pi_v<float> / 6.0f;
	auto const cam_rot = glm::mat4{glm::vec4{std::cos(cam_y), 0.0f, -std::sin(cam_y), 0.0f},
	                               glm::vec4{0.0f, 1.0f, 0.0f, 0.0f},
	                               glm::vec4{std::sin(cam_y), 0.0f, std::cos(cam_y), 0.0f},
	                               glm::vec4{0.0f, 0.0f, 0.0f, 1.0f}}
	                     * glm::mat4{glm::vec4{1.0f, 0.0f, 0.0f, 0.0f},
	                                 glm::vec4{0.0f, std::cos(cam_x), -std::sin(cam_x), 0.0f},
	                                 glm::vec4{0.0f, std::sin(cam_x), std::cos(cam_x), 0.0f},
	                                 glm::vec4{1.0f, 0.0f, 0.0f, 1.0f}};
	glUniformMatrix4fv(7, 1, GL_FALSE, glm::value_ptr(cam_rot));

	GLuint id{};
	glCreateVertexArrays(1, &id);
	glEnableVertexArrayAttrib(id, 0);
	glEnableVertexArrayAttrib(id, 1);
	glClearColor(0, 0, 0, 1.0);
	m_vert_array = VertexArray{GlHandle{id}};
}

namespace
{
	std::vector<std::pair<float, float>> gen_xy(Texpainter::Size2d size)
	{
		std::vector<std::pair<float, float>> ret(area(size));
		auto const r_x = 0.5f * static_cast<float>(size.width());
		auto const r_y = 0.5f * static_cast<float>(size.height());
		for(uint32_t k = 0; k != size.height(); ++k)
		{
			for(uint32_t l = 0; l != size.width(); ++l)
			{
				auto const x              = static_cast<float>(l);
				auto const y              = static_cast<float>(k);
				ret[k * size.width() + l] = std::pair{x - r_x + 0.5f, y - r_y + 0.5f};
			}
		}

		return ret;
	}

	std::vector<std::array<unsigned int, 3>> gen_faces(Texpainter::Size2d size)
	{
		std::vector<std::array<unsigned int, 3>> ret(2 * (size.width() - 1) * (size.height() - 1));

		auto k_prev         = 0;
		size_t write_offset = 0;

		for(uint32_t k = 1; k != size.height(); ++k)
		{
			auto l_prev = 0;
			for(uint32_t l = 1; l != size.width(); ++l)
			{
				ret[write_offset + 0] = std::array<unsigned int, 3>{k_prev * size.width() + l_prev,
				                                                    k_prev * size.width() + l,
				                                                    k * size.width() + l};

				ret[write_offset + 1] = std::array<unsigned int, 3>{k * size.width() + l,
				                                                    k * size.width() + l_prev,
				                                                    k_prev * size.width() + l_prev};

				write_offset += 2;
				l_prev = l;
			}
			k_prev = k;
		}

		return ret;
	}
}

Texpainter::App::TerrainView& Texpainter::App::TerrainView::meshSize(Size2d size)
{
	if((area(size) != area(m_mesh_size) || m_xy == nullptr) && m_initialized)
	{
		m_gl_area.activate();
		GLuint xy_id{};
		{
			glCreateBuffers(1, &xy_id);
			auto xy_data = gen_xy(size);
			glNamedBufferStorage(
			    xy_id, 2 * sizeof(float) * std::size(xy_data), std::data(xy_data), 0);
		}

		GLuint faces_id{};
		{
			glCreateBuffers(1, &faces_id);
			auto faces_data = gen_faces(size);
			glNamedBufferStorage(faces_id,
			                     3 * sizeof(unsigned int) * std::size(faces_data),
			                     std::data(faces_data),
			                     0);
		}

		GLuint topo_id{};
		glCreateBuffers(1, &topo_id);
		glNamedBufferStorage(
		    topo_id, sizeof(Model::TopographyInfo) * area(size), nullptr, GL_DYNAMIC_STORAGE_BIT);

		glVertexArrayVertexBuffer(*m_vert_array.get(), 0, xy_id, 0, 2 * sizeof(float));
		glVertexArrayVertexBuffer(
		    *m_vert_array.get(), 1, topo_id, 0, sizeof(Model::TopographyInfo));
		glVertexArrayAttribFormat(*m_vert_array.get(), 0, 2, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribFormat(*m_vert_array.get(), 1, 4, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(*m_vert_array.get(), 0, 0);
		glVertexArrayAttribBinding(*m_vert_array.get(), 1, 1);
		glVertexArrayElementBuffer(*m_vert_array.get(), faces_id);
		glBindVertexArray(*m_vert_array.get());
		glUniform1f(2, static_cast<float>(std::max(size.width(), size.height())));
		m_mesh_size = size;
		m_xy        = VertexBuffer{GlHandle{xy_id}};
		m_topo      = VertexBuffer{GlHandle{topo_id}};
		m_faces     = VertexBuffer{GlHandle{faces_id}};
		m_gl_area.redraw();
	}

	return *this;
}

Texpainter::App::TerrainView& Texpainter::App::TerrainView::topography(
    Span2d<Model::TopographyInfo const> n_elev)
{
	if(m_mesh_size != n_elev.size()) { meshSize(n_elev.size()); }

	if(m_xy == nullptr) { return *this; }

	auto peak = std::ranges::max_element(n_elev, [](auto a, auto b) {
		            return a.elevation() < b.elevation();
	            })->elevation();
	glUniform1f(
	    2, std::max(static_cast<float>(std::max(m_mesh_size.width(), m_mesh_size.height())), peak));

	m_gl_area.activate();

	glNamedBufferSubData(*m_topo.get(),
	                     0,
	                     static_cast<GLsizei>(sizeof(Model::TopographyInfo) * area(n_elev)),
	                     n_elev.data());
	m_gl_area.redraw();
	return *this;
}

template<>
void Texpainter::App::TerrainView::render<Texpainter::App::TerrainView::ControlId::GlArea>(
    Ui::GLArea const&)
{
	if(m_xy != nullptr)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		auto const face_count = 2 * (m_mesh_size.width() - 1) * (m_mesh_size.height() - 1);
		glDrawElements(
		    GL_TRIANGLES, 3 * static_cast<GLsizei>(face_count), GL_UNSIGNED_INT, nullptr);
	}
}