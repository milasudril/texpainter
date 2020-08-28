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
	class PortId
	{
	public:
		constexpr explicit PortId(uint64_t id): m_value{id} {}

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

		constexpr PortId& operator+=(uint64_t offset)
		{
			m_value += offset;
			return *this;
		}

		constexpr auto operator<=>(PortId const&) const = default;

	private:
		uint64_t m_value;
	};

	constexpr PortId operator+(PortId a, uint64_t offset) { return a += offset; }

	template<class Mesh, class IdArrayIterator>
	class ConnectorMove
	{
	public:
		explicit ConnectorMove(Mesh& connectors, IdArrayIterator iter)
		    : r_connectors{connectors}
		    , m_iter{iter}
		{
		}

		void operator()(auto const& item)
		{
			r_connectors.moveTo(*m_iter, item.location());
			++m_iter;
		}

	private:
		Mesh& r_connectors;
		IdArrayIterator m_iter;
	};

	struct PortMap
	{
		void addPorts(FilterGraph::Node& node);
		void addConnections(FilterGraph::Node const& node);

		void addConnection(FilterGraph::Endpoint<FilterGraph::InputPort> const& in,
		                   FilterGraph::Endpoint<FilterGraph::OutputPort> const& out)
		{
			m_connectors.connect(m_input_port_map.find(&in.node())->second[in.port().value()],
			                     m_output_port_map.find(&out.node())->second[out.port().value()]);
		}

		template<class InputConnectorList, class OutputConnectorList>
		void updateLocation(FilterGraph::Node const& node,
		                    InputConnectorList const& inputs,
		                    OutputConnectorList const& outputs)
		{
			{
				auto i = m_input_port_map.find(&node);
				assert(i != std::end(m_input_port_map));
				std::ranges::for_each(inputs, ConnectorMove{m_connectors, i->second.begin()});
			}

			{
				auto o = m_output_port_map.find(&node);
				assert(o != std::end(m_output_port_map));
				std::ranges::for_each(outputs, ConnectorMove{m_connectors, o->second.begin()});
			}
		}

		PortId m_current_port_id{0};

		DynamicMesh<PortId, Ui::ToplevelCoordinates> m_connectors;
		std::map<FilterGraph::Node const*, std::vector<PortId>> m_input_port_map;
		std::map<FilterGraph::Node const*, std::vector<PortId>> m_output_port_map;
	};


	class FilterGraphEditor
	{
		using Canvas     = Ui::WidgetCanvas<FilterGraph::NodeId>;
		using NodeWidget = NodeEditor<FilterGraphEditor>;

	public:
		enum class ControlId : int
		{
			NodeWidgets,
			CopyNode,
			DeleteNode
		};

		FilterGraphEditor(Ui::Container& owner, FilterGraph::Graph& graph);

		FilterGraphEditor& insert(std::unique_ptr<FilterGraph::AbstractImageProcessor> node);

		template<ControlId>
		void onMouseDown(Canvas& src,
		                 Ui::WidgetCoordinates,
		                 Ui::ScreenCoordinates,
		                 int button,
		                 FilterGraph::NodeId);

		template<ControlId>
		void onRealized(Canvas& src);

		template<ControlId>
		void onMove(Canvas& src, Ui::WidgetCoordinates, FilterGraph::NodeId);

		template<ControlId>
		void onActivated(Ui::MenuItem& src);

		void onClicked(NodeWidget const& src, FilterGraph::InputPort port)
		{
			if(m_con_proc == nullptr)
			{
				//TODO: Start track mouse pointer and draw line from current port to mouse
				//      cursor
				m_con_proc = std::make_unique<FilterGraph::Connection>(src.node(), port);
			}
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
			{
				//TODO: Start track mouse pointer and draw line from current port to mouse
				//      cursor
				m_con_proc = std::make_unique<FilterGraph::Connection>(src.node(), port);
			}
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

		void notCompleted(FilterGraph::Connection const&) {}

		void selfConnection(FilterGraph::Connection const&) {}

		void typeMismatch(FilterGraph::Connection const&) {}

		void connectionOk(FilterGraph::Connection const& conn)
		{
			establish(conn);
			m_ports.addConnection(conn.sink(), conn.source());
			// TODO: Remove any conflicting connections

			m_linesegs->lineSegments(resolveLineSegs(m_ports.m_connectors));
		}

		void updateLocations();

	private:
		FilterGraph::Graph& r_graph;

		PortMap m_ports;

		FilterGraph::NodeId m_sel_node;

		Canvas m_canvas;
		std::unique_ptr<Ui::LineSegmentRenderer> m_linesegs;
		std::map<FilterGraph::NodeId, Canvas::WidgetHandle<NodeWidget>> m_node_editors;
		Ui::Menu m_node_menu;
		Ui::MenuItem m_node_copy;
		Ui::MenuItem m_node_delete;

		std::unique_ptr<FilterGraph::Connection> m_con_proc;


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
	inline void FilterGraphEditor::onActivated<FilterGraphEditor::ControlId::CopyNode>(
	    Ui::MenuItem&)
	{
		insert(r_graph.node(m_sel_node)->clonedProcessor());
	}

	template<>
	inline void FilterGraphEditor::onActivated<FilterGraphEditor::ControlId::DeleteNode>(
	    Ui::MenuItem&)
	{
		auto node = r_graph.node(m_sel_node);

		std::ranges::for_each(
		    m_ports.m_output_port_map.find(node)->second,
		    [&connections = m_ports.m_connectors](auto item) { connections.remove(item); });
		m_ports.m_output_port_map.erase(node);

		std::ranges::for_each(
		    m_ports.m_input_port_map.find(node)->second,
		    [&connections = m_ports.m_connectors](auto item) { connections.remove(item); });
		m_ports.m_input_port_map.erase(node);

		m_node_editors.erase(m_sel_node);

		r_graph.erase(m_sel_node);
		m_linesegs->lineSegments(resolveLineSegs(m_ports.m_connectors));
	}

	template<>
	inline void FilterGraphEditor::onRealized<FilterGraphEditor::ControlId::NodeWidgets>(Canvas&)
	{
		updateLocations();
	}
}

#endif