//@	{
//@	 "targets":[{"name":"filter_graph_editor.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"filter_graph_editor.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_APP_FILTERGRAPHEDITOR_HPP
#define TEXPAINTER_APP_FILTERGRAPHEDITOR_HPP

#include "./node_editor.hpp"

#include "filtergraph/graph.hpp"
#include "filtergraph/connection.hpp"
#include "ui/widget_canvas.hpp"
#include "ui/line_segment_renderer.hpp"
#include "ui/menu.hpp"
#include "ui/menu_item.hpp"
#include "utils/dynamic_mesh.hpp"

#include <limits>
#include <cassert>

namespace Texpainter
{
	class FilterGraphEditor
	{
		using Canvas = Ui::WidgetCanvas<FilterGraph::NodeId>;
		class PortId
		{
		public:
			constexpr PortId(): m_value{0} {}

			constexpr explicit PortId(uint64_t id): m_value{id} { assert(valid()); }

			constexpr bool valid() const { return m_value != 0; }

			constexpr uint64_t value() const { return m_value; }

			constexpr PortId operator++(int)
			{
				auto tmp = *this;
				++m_value;
				return tmp;
			}

			constexpr PortId& operator++()
			{
				++m_value;
				return *this;
			}

			constexpr auto operator<=>(PortId const&) const = default;

		private:
			uint64_t m_value;
		};


		using NodeWidget = NodeEditor<FilterGraphEditor>;

	public:
		enum class ControlId : int
		{
			NodeWidgets,
			CopyNode,
			DeleteNode
		};


		FilterGraphEditor(Ui::Container& owner, FilterGraph::Graph& graph)
		    : r_graph{graph}
		    , m_canvas{owner}
		    , m_node_copy{m_node_menu, "Copy"}
		    , m_node_delete{m_node_menu, "Delete"}
		{
			m_linesegs = m_canvas.insert<Ui::LineSegmentRenderer>();
			std::ranges::transform(r_graph.nodes(),
			                       std::inserter(m_node_editors, std::end(m_node_editors)),
			                       [&canvas = m_canvas](auto& node) {
				                       return std::make_pair(node.first,
				                                             canvas.insert<NodeWidget>(
				                                                 node.first,
				                                                 Ui::WidgetCoordinates{50.0, 50.0},
				                                                 node.second));
			                       });

			m_canvas.eventHandler<ControlId::NodeWidgets>(*this);
			std::ranges::for_each(m_node_editors,
			                      [this](auto& item) { item.second->eventHandler(*this); });
			m_node_copy.eventHandler<ControlId::CopyNode>(*this);
			m_node_delete.eventHandler<ControlId::DeleteNode>(*this);
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

		void onClicked(NodeWidget const& src, FilterGraph::InputPort port)
		{
			if(m_con_proc == nullptr)
			{ m_con_proc = std::make_unique<FilterGraph::Connection>(src.node(), port); }
			else if(m_con_proc->sink().valid())
			{
				m_con_proc = std::make_unique<FilterGraph::Connection>(src.node(), port);
			}
			else
			{
				m_con_proc->sink(src.node(), port);
				completeConnection();
			}
		}

		void onClicked(NodeWidget const& src, FilterGraph::OutputPort port)
		{
			if(m_con_proc == nullptr)
			{ m_con_proc = std::make_unique<FilterGraph::Connection>(src.node(), port); }
			else if(m_con_proc->source().valid())
			{
				m_con_proc = std::make_unique<FilterGraph::Connection>(src.node(), port);
			}
			else
			{
				m_con_proc->source(src.node(), port);
				completeConnection();
			}
		}

		void notCompleted(FilterGraph::Connection const&)
		{}

		void selfConnection(FilterGraph::Connection const&)
		{}

		void typeMismatch(FilterGraph::Connection const&)
		{}

		void connectionOk(FilterGraph::Connection const& conn)
		{
			establish(conn);

			m_connections.insert(std::make_pair(m_current_port_id, Ui::ToplevelCoordinates{0.0, 0.0}));
			m_input_port_map.find(&conn.sink().node())->second[conn.sink().port().value()] = m_current_port_id;
			++m_current_port_id;

			m_connections.insert(std::make_pair(m_current_port_id, Ui::ToplevelCoordinates{0.0, 0.0}));
			m_output_port_map.find(&conn.source().node())->second[conn.source().port().value()] = m_current_port_id;
			++m_current_port_id;

			m_linesegs->lineSegments(resolveLineSegs(m_connections));
		}


	private:
		FilterGraph::Graph& r_graph;
		PortId m_current_port_id;
		DynamicMesh<PortId, Ui::ToplevelCoordinates> m_connections;
		std::map<FilterGraph::Node const*, std::vector<PortId>> m_input_port_map;
		std::map<FilterGraph::Node const*, std::vector<PortId>> m_output_port_map;
		FilterGraph::NodeId m_sel_node;

		Canvas m_canvas;
		std::unique_ptr<Ui::LineSegmentRenderer> m_linesegs;
		std::map<FilterGraph::NodeId, Canvas::WidgetHandle<NodeWidget>> m_node_editors;
		Ui::Menu m_node_menu;
		Ui::MenuItem m_node_copy;
		Ui::MenuItem m_node_delete;

		std::unique_ptr<FilterGraph::Connection> m_con_proc;

		void init();

		void completeConnection()
		{
			validate(*m_con_proc, *this);
			m_con_proc.reset();
		}
	};

	template<>
	inline void FilterGraphEditor::onMouseDown<FilterGraphEditor::ControlId::NodeWidgets>(
	    Canvas& src,
	    Ui::WidgetCoordinates,
	    Ui::ScreenCoordinates,
	    int button,
	    FilterGraph::NodeId node)
	{
		if(button == 3)
		{
			if(node != FilterGraph::Graph::InputNodeId && node != FilterGraph::Graph::OutputNodeId)
			{
				m_sel_node = node;
				m_node_menu.show().popupAtCursor();
			}
		}
	}

	template<>
	inline void FilterGraphEditor::onMove<FilterGraphEditor::ControlId::NodeWidgets>(
	    Canvas& src, Ui::WidgetCoordinates loc, FilterGraph::NodeId id)
	{
		if(!m_current_port_id.valid()) [[unlikely]]
			{
				init();
			}
		auto node_edit_iter = m_node_editors.find(id);
		assert(node_edit_iter != std::end(m_node_editors));

		auto const& node_edit = *(node_edit_iter->second);

		auto i = m_input_port_map.find(&node_edit.node());
		assert(i != std::end(m_input_port_map));
		std::ranges::for_each(node_edit.inputs(),
		                      [&connections = m_connections,
		                       &ids         = i->second,
		                       k            = static_cast<size_t>(0)](auto const& item) mutable {
			                      if(ids[k].valid())
			                      { connections.moveTo(ids[k], item.location()); }
			                      ++k;
		                      });

		auto o = m_output_port_map.find(&node_edit.node());
		assert(o != std::end(m_output_port_map));
		std::ranges::for_each(node_edit.outputs(),
		                      [&connections = m_connections,
		                       &ids         = o->second,
		                       k            = static_cast<size_t>(0)](auto const& item) mutable {
			                      if(ids[k].valid())
			                      { connections.moveTo(ids[k], item.location()); }
			                      ++k;
		                      });

		m_linesegs->lineSegments(resolveLineSegs(m_connections));
	}

	template<>
	inline void FilterGraphEditor::onActivated<FilterGraphEditor::ControlId::CopyNode>(
	    Ui::MenuItem&)
	{
		auto node = r_graph.insert(r_graph.node(m_sel_node)->clonedProcessor());

		// TODO: Below is similar to create new node
		m_node_editors.insert(
		    std::make_pair(node.first,
		                   m_canvas.insert<NodeWidget>(
		                       node.first, Ui::WidgetCoordinates{50.0, 50.0}, node.second)));
		m_canvas.showWidgets();
		m_input_port_map.insert(std::make_pair(
		    &node.second.get(), std::vector<PortId>(node.second.get().inputPorts().size())));
		m_output_port_map.insert(std::make_pair(
		    &node.second.get(), std::vector<PortId>(node.second.get().outputPorts().size())));
	}

	template<>
	inline void FilterGraphEditor::onActivated<FilterGraphEditor::ControlId::DeleteNode>(
	    Ui::MenuItem&)
	{
		auto node = r_graph.node(m_sel_node);

		std::ranges::for_each(m_output_port_map.find(node)->second,
		                      [&connections = m_connections](auto item) {
			                      if(item.valid()) { connections.remove(item); }
		                      });
		m_output_port_map.erase(node);

		std::ranges::for_each(m_input_port_map.find(node)->second,
		                      [&connections = m_connections](auto item) {
			                      if(item.valid()) { connections.remove(item); }
		                      });
		m_input_port_map.erase(node);

		m_node_editors.erase(m_sel_node);

		r_graph.erase(m_sel_node);
		m_linesegs->lineSegments(resolveLineSegs(m_connections));
	}
}

#endif