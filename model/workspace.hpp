//@	{
//@	 "targets":[{"name":"workspace.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"workspace.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_MODEL_WORKSPACE_HPP
#define TEXPAINTER_MODEL_WORKSPACE_HPP

#include "./brush.hpp"
#include "./item_name.hpp"
#include "./window_type.hpp"

#include "filtergraph/graph.hpp"

#include <nlohmann/json.hpp>

#include <filesystem>
#include <optional>

namespace Texpainter::Model
{
	struct WindowRectangle
	{
		Size2d size{0, 0};
		vec2_t location{0.0, 0.0};
	};

	void to_json(nlohmann::json& obj, WindowRectangle const& workspace);

	enum class WindowState : int
	{
		Normal,
		Minimized,
		Maximized
	};

	void to_json(nlohmann::json& obj, WindowState state);

	struct Window
	{
		bool visible{false};
		WindowState state{WindowState::Normal};
		WindowRectangle rect{};
	};

	void to_json(nlohmann::json& obj, Window const& window);


	template<WindowType>
	struct WindowTypeInfo;

	template<>
	struct WindowTypeInfo<WindowType::Compositor>
	{
		using type = Window;
		static constexpr char const* name() { return "compositor"; }
	};

	template<>
	struct WindowTypeInfo<WindowType::ImageEditor>
	{
		using type = Window;
		static constexpr char const* name() { return "image_editor"; }
	};

	template<>
	struct WindowTypeInfo<WindowType::DocumentPreviewer>
	{
		using type = Window;
		static constexpr char const* name() { return "document_previewer"; }
	};

	using Windows = Enum::Tuple<WindowType, WindowTypeInfo>;

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