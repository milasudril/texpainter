//@	{
//@	 "targets":
//@		[{
//@		"name":"terrain_view.o",
//@		"type":"object"
//@		}]
//@	}

#include "./terrain_view.hpp"
#include "log/logger.hpp"

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

out vec4 vertex_normal;
out vec4 frag_pos;

void main()
{
	vec4 pos = vec4(xy.xy, n_elev.w, 1.0);
	gl_Position = pos;
	frag_pos = pos;
	vertex_normal = vec4(n_elev.xyz, 0.0);
}
)shader";

	constexpr char const* frag_shader = R"shader(#version 460 core
layout(location = 0) out vec4 frag_color;

in vec4 vertex_normal;
in vec4 frag_pos;

void main()
{
	frag_color = vec4(1.0, 0.5, 0.25, 1.0);
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
				ret[k * size.width() + l] = std::pair{x - r_x, y - r_y};
			}
		}

		return ret;
	}

	std::vector<std::array<unsigned int, 3>> gen_faces(Texpainter::Size2d size)
	{
		std::vector<std::array<unsigned int, 3>> ret(2 * (size.width() - 1) * (size.height() - 1));

		auto k_prev         = 0;
		auto l_prev         = 0;
		size_t write_offset = 0;

		for(uint32_t k = 1; k != size.height(); ++k)
		{
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

		m_mesh_size = size;
		m_xy        = VertexBuffer{GlHandle{xy_id}};
		m_topo      = VertexBuffer{GlHandle{topo_id}};
		m_faces     = VertexBuffer{GlHandle{faces_id}};
	}

	return *this;
}

template<>
void Texpainter::App::TerrainView::render<Texpainter::App::TerrainView::ControlId::GlArea>(
    Ui::GLArea const&)
{
	glClear(GL_COLOR_BUFFER_BIT);
	puts("Render");
}