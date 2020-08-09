//@	{
//@	"targets":[{"name":"filter_graph_editor.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPHEDITOR_HPP
#define TEXPAINTER_FILTERGRAPHEDITOR_HPP

#include "./node_editor.hpp"

#include "filtergraph/graph.hpp"
#include "ui/widget_canvas.hpp"
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
			NodeEditors
		};


		FilterGraphEditor(Ui::Container& owner, FilterGraph::Graph& graph)
		    : r_graph{graph}
		    , m_current_port_id{0}
		    , m_canvas{owner}
		{
			std::ranges::transform(r_graph.nodes(),
			                       std::inserter(m_node_editors, std::end(m_node_editors)),
			                       [&canvas = m_canvas](auto const& node) {
				                       return std::make_pair(node.first,
				                                             canvas.insert<NodeEditor>(
				                                                 node.first,
				                                                 Ui::WidgetCoordinates{50.0, 50.0},
				                                                 node.second));
			                       });

			std::ranges::for_each(
			    m_node_editors,
			    [&port_id      = m_current_port_id,
			     &connections  = m_connections,
			     &port_to_node = m_port_to_node](auto const& item) {
				    auto new_port_id = add_ports_from(*item.second, port_id, connections);
				    map_ports_to_node(port_id, new_port_id, port_to_node, item.second->node());
				    port_id = new_port_id;
			    });
#if 0
			std::ranges::for_each(m_connections,
			                      [&connections  = m_connections,
			                       &port_to_node = std::as_const(m_port_to_node),
			                       &graph        = std::as_const(r_graph)](auto const& item) {
				                      auto i = port_to_node.find(item.first);
				                      assert(i != std::end(port_to_node));
				                      auto node = graph.node(i->second);
				                      assert(node != nullptr);
									  auto inputs = node->inputs();
				                      connections.connect(item.first);
			                      });
#endif

			m_canvas.eventHandler<ControlId::NodeEditors>(*this);
		}

		template<ControlId>
		void onMouseDown(Canvas& src,
		                 Ui::WidgetCoordinates,
		                 Ui::ScreenCoordinates,
		                 int button,
		                 FilterGraph::NodeId);

		template<ControlId>
		void onMove(Canvas& src, Ui::WidgetCoordinates, FilterGraph::NodeId);

	private:
		FilterGraph::Graph& r_graph;
		uint64_t m_current_port_id;
		DynamicMesh<uint64_t, Ui::ToplevelCoordinates> m_connections;
		std::map<uint64_t, std::reference_wrapper<FilterGraph::ProcessorNode const>> m_port_to_node;

		Canvas m_canvas;
		std::map<FilterGraph::NodeId, Canvas::WidgetHandle<NodeEditor>> m_node_editors;

		template<class PortCollection>
		static void add_ports_from(PortCollection const& ports,
		                           uint64_t& port_id,
		                           DynamicMesh<uint64_t, Ui::ToplevelCoordinates>& connections)
		{
			std::ranges::for_each(ports, [&connections, &port_id](auto const& port) {
				connections.insert(std::make_pair(port_id, port.inputField().location()));
				++port_id;
			});
		}

		static uint64_t add_ports_from(NodeEditor const& node_edit,
		                               uint64_t port_id_start,
		                               DynamicMesh<uint64_t, Ui::ToplevelCoordinates>& connections)
		{
			add_ports_from(node_edit.inputs(), port_id_start, connections);
			add_ports_from(node_edit.outputs(), port_id_start, connections);
			return port_id_start;
		}

		static void map_ports_to_node(
		    uint64_t first_port,
		    uint64_t after_last_port,
		    std::map<uint64_t, std::reference_wrapper<FilterGraph::ProcessorNode const>>&
		        port_to_node,
		    std::reference_wrapper<FilterGraph::ProcessorNode const> node)
		{
			while(first_port != after_last_port)
			{
				port_to_node.insert(std::make_pair(first_port, node));
				++first_port;
			}
		}
	};

	template<>
	inline void FilterGraphEditor::onMouseDown<FilterGraphEditor::ControlId::NodeEditors>(
	    Canvas& src,
	    Ui::WidgetCoordinates,
	    Ui::ScreenCoordinates,
	    int button,
	    FilterGraph::NodeId node)
	{
		if(button == 3) { printf("%zu\n", node.value()); }
	}

	template<>
	inline void FilterGraphEditor::onMove<FilterGraphEditor::ControlId::NodeEditors>(
	    Canvas& src, Ui::WidgetCoordinates loc, FilterGraph::NodeId id)
	{
		printf("%zu moved to %.7f %.7f\n", id.value(), loc.x(), loc.y());
		auto const& node = *(m_node_editors.find(id)->second);
		std::ranges::for_each(node.inputs(), [](auto const& item) {
			auto loc = item.inputField().location();
			printf("  %s (%.7f %.7f)\n", item.label().content(), loc.x(), loc.y());
		});

		std::ranges::for_each(node.outputs(), [](auto const& item) {
			auto loc = item.inputField().location();
			printf("  %s (%.7f %.7f)\n", item.label().content(), loc.x(), loc.y());
		});
	}

}

#endif
