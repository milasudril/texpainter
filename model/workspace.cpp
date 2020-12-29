//@	{
//@	 "targets":[{"name":"workspace.o", "type":"object"}]
//@	}

#include "./workspace.hpp"

void Texpainter::Model::to_json(nlohmann::json& obj, Workspace const& workspace)
{
	// TODO:
	obj = nlohmann::json{std::pair{"node_locations", workspace.m_node_locations}};
}