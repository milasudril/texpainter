//@	{
//@	 "targets":[{"name":"workspace.o", "type":"object"}]
//@	}

#include "./workspace.hpp"

void Texpainter::Model::to_json(nlohmann::json& obj, WindowRectangle const& rect)
{
	obj = nlohmann::json{std::pair{"size", rect.size}, std::pair{"location", rect.location}};
}

void Texpainter::Model::from_json(nlohmann::json const& obj, WindowRectangle& rect)
{
	rect.size = obj.at("size").get<Size2d>();
	rect.location = obj.at("location").get<vec2_t>();
}

void Texpainter::Model::to_json(nlohmann::json& obj, WindowState state)
{
	obj["minimized"] = state.minimized();
	obj["maximized"] = state.maximized();
}

void Texpainter::Model::from_json(nlohmann::json const& obj, WindowState& state)
{
	state.minimized(obj.at("minimized").get<bool>())
		.maximized(obj.at("maximized").get<bool>());
}

void Texpainter::Model::to_json(nlohmann::json& obj, Window const& window)
{
	obj = nlohmann::json{std::pair{"visible", window.visible},
	                     std::pair{"state", window.state},
	                     std::pair{"rect", window.rect}};
}

void Texpainter::Model::from_json(nlohmann::json const& obj, Window& window)
{
	window.visible = obj.at("visible").get<bool>();
	window.state = obj.at("state").get<WindowState>();
	window.rect = obj.at("rect").get<WindowRectangle>();
}

void Texpainter::Model::to_json(nlohmann::json& obj, Windows const& windows)
{
	Enum::forEachEnumItem<WindowType>([&obj, &windows](auto item) {
		obj[WindowTypeInfo<item.value>::name()] = windows.get<item.value>();
	});
}

void Texpainter::Model::from_json(nlohmann::json const& obj, Windows& windows)
{
	Enum::forEachEnumItem<WindowType>([&obj, &windows](auto item) {
		windows.get<item.value>() = obj.at(WindowTypeInfo<item.value>::name()).template get<Window>();
	});
}

void Texpainter::Model::to_json(nlohmann::json& obj, Workspace const& workspace)
{
	obj = nlohmann::json{std::pair{"node_locations", workspace.m_node_locations},
	                     std::pair{"current_image", workspace.m_current_image},
	                     std::pair{"current_color", workspace.m_current_color},
	                     std::pair{"current_palette", workspace.m_current_palette},
	                     std::pair{"color_history", workspace.m_color_history},
	                     std::pair{"windows", workspace.m_windows},
	                     std::pair{"current_brush", workspace.m_current_brush}};
}

void Texpainter::Model::from_json(nlohmann::json const& obj, Workspace& workspace)
{
	workspace.m_node_locations = obj.at("node_locations").get<std::map<FilterGraph::NodeId, vec2_t>>();
	workspace.m_current_image = obj.at("current_image").get<ItemName>();
	workspace.m_current_color = obj.at("current_color").get<PixelStore::ColorIndex>();
	workspace.m_current_palette = obj.at("current_palette").get<ItemName>();
	workspace.m_color_history = obj.at("color_history").get<PixelStore::Palette<8>>();
	workspace.m_windows = obj.at("windows").get<Windows>();
	workspace.m_current_brush = obj.at("current_brush").get<BrushInfo>();
}