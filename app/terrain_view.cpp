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
void Texpainter::App::TerrainView::realize<Texpainter::App::TerrainView::ControlId::GlArea>(Ui::GLArea const&)
{
	glewExperimental = true;
	if(auto err = glewInit(); err != GLEW_OK)
	{
		Logger::log(Logger::MessageType::Error, reinterpret_cast<char const*>(glewGetErrorString(err)));
		return;
	}

	Logger::log(Logger::MessageType::Info,  reinterpret_cast<char const*>(glGetString(GL_VERSION)));
	// TODO: Create shader program
}