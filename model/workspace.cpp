//@	{
//@	 "targets":[{"name":"workspace.o", "type":"object"}]
//@	}

#include "./workspace.hpp"

void Texpainter::Model::to_json(nlohmann::json& obj, Workspace const& workspace)
{
	obj = nlohmann::json{std::pair{"node_locations", workspace.m_node_locations},
	                     std::pair{"current_image", workspace.m_current_image},
	                     std::pair{"current_color", workspace.m_current_color},
	                     std::pair{"current_palette", workspace.m_current_palette},
	                     std::pair{"color_history", workspace.m_color_history}};
}