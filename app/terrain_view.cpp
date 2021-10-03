//@	{
//@	 "targets":
//@		[{
//@		"name":"terrain_view.o",
//@		"type":"object"
//@		}]
//@	}

#include "./terrain_view.hpp"
#include "log/logger.hpp"

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

	Logger::log(Logger::MessageType::Info, reinterpret_cast<char const*>(glGetString(GL_VERSION)));
	// TODO: Create shader program
}

std::vector<std::pair<float, float>> gen_xy(Texpainter::Size2d size)
{
	std::vector<std::pair<float, float>> ret(area(size));

	for(uint32_t k = 0; k != size.height(); ++k)
	{
		for(uint32_t l = 0; l != size.width(); ++l)
		{
			ret[k * size.width() + l] =
			    std::pair{static_cast<float>(l - l / 2), static_cast<float>(k - k / 2)};
		}
	}

	return ret;
}

std::vector<std::array<unsigned int, 3>> gen_faces(Texpainter::Size2d size)
{
	std::vector<std::array<unsigned int, 3>> ret(area(size));
#if 0
	for(uint32_t k = 0; k != size.height(); ++k)
	{
		for(uint32_t l = 0; l != size.width(); ++l)
		{
			ret[k * size.width() + l] =
			    std::pair{static_cast<float>(l - l / 2), static_cast<float>(k - k / 2)};
		}
	}
#endif
	return ret;
}

Texpainter::App::TerrainView& Texpainter::App::TerrainView::meshSize(Size2d size)
{
	if(area(size) != area(m_mesh_size) || m_xy == nullptr)
	{
		GLuint xy_id{};
		{
			glGenBuffers(1, &xy_id);
			auto xy_data = gen_xy(size);
			glNamedBufferStorage(
			    xy_id, 2 * sizeof(float) * std::size(xy_data), std::data(xy_data), 0);
		}

		GLuint faces_id{};
		{
			glGenBuffers(1, &faces_id;
			auto faces_data = gen_faces(size);
			glNamedBufferStorage(faces_id, 3 * sizeof(unsigned int) * std::size(faces_data), std::data(faces_data), 0);
		}

		GLuint topo_id{};
		glGenBuffers(1, &topo_id);
		glNamedBufferStorage(topo_id,
		                     sizeof(Model::TopographyInfo) * std::size(xy_data),
		                     nullptr,
		                     GL_DYNAMIC_STORAGE_BIT);

		m_mesh_size = size;
		m_xy        = VertexBuffer{GlHandle{xy_id}};
		m_topo      = VertexBuffer{GlHandle{topo_id}};
		m_faces     = VertexBuffer{GlHandle{faces_id}};
	}

	return *this;
}