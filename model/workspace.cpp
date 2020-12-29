//@	{
//@	 "targets":[{"name":"workspace.o", "type":"object"}]
//@	}

#include "./workspace.hpp"

void Texpainter::Model::to_json(nlohmann::json&, Workspace const&)
{
	// TODO:
	//	obj = nlohmann::json{std::pair{"node_locations", workspace.m_node_locations}};
}