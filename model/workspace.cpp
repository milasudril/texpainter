//@	{
//@	 "targets":[{"name":"workspace.o", "type":"object"}]
//@	}

#include "./workspace.hpp"

void Texpainter::Model::to_json(nlohmann::json& obj, WindowRectangle const& rect)
{
	obj = nlohmann::json{std::pair{"size", rect.size}, std::pair{"location", rect.location}};
}

void Texpainter::Model::to_json(nlohmann::json& obj, WindowState state)
{
	obj["minimized"] = state.minimized();
	obj["maximized"] = state.maximized();
}

void Texpainter::Model::to_json(nlohmann::json& obj, Window const& window)
{
	obj = nlohmann::json{std::pair{"visible", window.visible},
	                     std::pair{"state", window.state},
	                     std::pair{"rect", window.rect}};
}

void Texpainter::Model::to_json(nlohmann::json& obj, Windows const& windows)
{
	Enum::forEachEnumItem<WindowType>([&obj, &windows](auto item) {
		obj[WindowTypeInfo<item.value>::name()] = windows.get<item.value>();
	});
}

void Texpainter::Model::to_json(nlohmann::json& obj, Workspace const& workspace)
{
	obj = nlohmann::json{std::pair{"node_locations", workspace.m_node_locations},
	                     std::pair{"current_image", workspace.m_current_image},
	                     std::pair{"current_color", workspace.m_current_color},
	                     std::pair{"current_palette", workspace.m_current_palette},
	                     std::pair{"color_history", workspace.m_color_history},
	                     std::pair{"windows", workspace.m_windows}};
}