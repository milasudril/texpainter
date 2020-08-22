//@	{
//@	 "targets":[{"name":"filter_graph_editor.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"filter_graph_editor.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_APP_FILTERGRAPHEDITOR_HPP
#define TEXPAINTER_APP_FILTERGRAPHEDITOR_HPP

#include "./node_editor.hpp"

#include "filtergraph/graph.hpp"
#include "ui/widget_canvas.hpp"
#include "ui/line_segment_renderer.hpp"
#include "ui/menu.hpp"
#include "ui/menu_item.hpp"
#include "utils/dynamic_mesh.hpp"

#include <cassert>

namespace Texpainter
{
	class FilterGraphEditor
	{
		using Canvas = Ui::WidgetCanvas<FilterGraph::NodeId>;

	public:
		enum class ControlId : int
		{
			NodeEditors,
			CopyNode
		};


		FilterGraphEditor(Ui::Container& owner, FilterGraph::Graph& graph)
		    : r_graph{graph}
		    , m_current_port_id{0}
		    , m_canvas{owner}
		    , m_node_copy{m_node_menu, "Copy"}
		{
			m_linesegs = m_canvas.insert<Ui::LineSegmentRenderer>();
			std::ranges::transform(r_graph.nodes(),
			                       std::inserter(m_node_editors, std::end(m_node_editors)),
			                       [&canvas = m_canvas](auto const& node) {
				                       return std::make_pair(node.first,
				                                             canvas.insert<NodeEditor>(
				                                                 node.first,
				                                                 Ui::WidgetCoordinates{50.0, 50.0},
				                                                 node.second));
			                       });
			m_canvas.eventHandler<ControlId::NodeEditors>(*this);
			m_node_copy.eventHandler<ControlId::CopyNode>(*this);
		}

		template<ControlId>
		void onMouseDown(Canvas& src,
		                 Ui::WidgetCoordinates,
		                 Ui::ScreenCoordinates,
		                 int button,
		                 FilterGraph::NodeId);

		template<ControlId>
		void onMove(Canvas& src, Ui::WidgetCoordinates, FilterGraph::NodeId);

		template<ControlId>
		void onActivated(Ui::MenuItem& src);

	private:
		FilterGraph::Graph& r_graph;
		uint64_t m_current_port_id;
		DynamicMesh<uint64_t, Ui::ToplevelCoordinates> m_connections;
		std::map<FilterGraph::Node const*, std::vector<uint64_t>> m_input_port_map;
		std::map<FilterGraph::Node const*, std::vector<uint64_t>> m_output_port_map;
		FilterGraph::NodeId m_sel_node;

		Canvas m_canvas;
		std::unique_ptr<Ui::LineSegmentRenderer> m_linesegs;
		std::map<FilterGraph::NodeId, Canvas::WidgetHandle<NodeEditor>> m_node_editors;
		Ui::Menu m_node_menu;
		Ui::MenuItem m_node_copy;


		void init();
	};

	template<>
	inline void FilterGraphEditor::onMouseDown<FilterGraphEditor::ControlId::NodeEditors>(
	    Canvas& src,
	    Ui::WidgetCoordinates,
	    Ui::ScreenCoordinates,
	    int button,
	    FilterGraph::NodeId node)
	{
		if(button == 3)
		{
			m_sel_node = node;
			m_node_menu.show().popupAtCursor();
		}
	}

	template<>
	inline void FilterGraphEditor::onMove<FilterGraphEditor::ControlId::NodeEditors>(
	    Canvas& src, Ui::WidgetCoordinates loc, FilterGraph::NodeId id)
	{
		if(m_current_port_id == 0) [[unlikely]]
			{
				init();
			}
		auto node_edit_iter = m_node_editors.find(id);
		assert(node_edit_iter != std::end(m_node_editors));

		auto const& node_edit = *(node_edit_iter->second);

		auto i = m_input_port_map.find(&node_edit.node().get());
		assert(i != std::end(m_input_port_map));
		std::ranges::for_each(node_edit.inputs(),
		                      [&connections = m_connections,
		                       &ids         = i->second,
		                       k            = static_cast<size_t>(0)](auto const& item) mutable {
			                      connections.moveTo(ids[k], item.inputField().location());
			                      ++k;
		                      });

		auto o = m_output_port_map.find(&node_edit.node().get());
		assert(o != std::end(m_output_port_map));
		std::ranges::for_each(node_edit.outputs(),
		                      [&connections = m_connections,
		                       &ids         = o->second,
		                       k            = static_cast<size_t>(0)](auto const& item) mutable {
			                      connections.moveTo(ids[k], item.inputField().location());
			                      ++k;
		                      });
		m_linesegs->lineSegments(resolveLineSegs(m_connections));
	}

	template<>
	inline void FilterGraphEditor::onActivated<FilterGraphEditor::ControlId::CopyNode>(
	    Ui::MenuItem&)
	{
		printf("Copy node %zu\n", m_sel_node.value());
	}
}

#endif