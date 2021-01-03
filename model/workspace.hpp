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

#include <filesystem>
#include <optional>

namespace Texpainter::Model
{
	struct WindowRectangle
	{
		Size2d size;
		vec2_t location;
	};

	void to_json(nlohmann::json& obj, WindowRectangle const& workspace);

	struct Windows
	{
		std::optional<WindowRectangle> image_editor;
		std::optional<WindowRectangle> compositor;
		std::optional<WindowRectangle> document_previewer;
	};

	void to_json(nlohmann::json& obj, Windows const& workspace);

	struct Workspace
	{
		Workspace()
		    : m_current_brush{BrushInfo{0.5f, BrushShape::Circle}}
		    , m_working_directory{std::filesystem::current_path()}
		{
		}

		std::map<FilterGraph::NodeId, vec2_t> m_node_locations;
		BrushInfo m_current_brush;
		ItemName m_current_image;
		PixelStore::ColorIndex m_current_color;
		ItemName m_current_palette;
		PixelStore::Palette<8> m_color_history;
		Windows m_windows;

		std::filesystem::path m_working_directory;
	};

	void to_json(nlohmann::json& obj, Workspace const& workspace);
}

#endif