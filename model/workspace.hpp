//@	{
//@	 "targets":[{"name":"workspace.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"workspace.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_MODEL_WORKSPACE_HPP
#define TEXPAINTER_MODEL_WORKSPACE_HPP

#include "./brush.hpp"
#include "./item_name.hpp"

#include "filtergraph/graph.hpp"

#include <nlohmann/json.hpp>

namespace Texpainter::Model
{
	struct Workspace
	{
		Workspace():m_current_brush{BrushInfo{0.5f, BrushShape::Circle}}{}

		std::map<FilterGraph::NodeId, vec2_t> m_node_locations;
		BrushInfo m_current_brush;
		ItemName m_current_image;
		PixelStore::ColorIndex m_current_color;
		ItemName m_current_palette;
		PixelStore::Palette<8> m_color_history;
	};

	void to_json(nlohmann::json& obj, Workspace const& workspace);
}

#endif