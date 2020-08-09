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
			    [&port_id         = m_current_port_id,
			     &connections     = m_connections,
			     &input_port_map  = m_input_port_map,
			     &output_port_map = m_output_port_map](auto const& item) {
				    auto port_id_start = port_id;
				    auto input_ports_end =
				        add_ports_from(item.second->inputs(), port_id_start, connections);
				    map_node_to_ports(
				        port_id_start, input_ports_end, input_port_map, item.second->node());

				    auto output_ports_end =
				        add_ports_from(item.second->outputs(), input_ports_end, connections);
				    map_node_to_ports(
				        input_ports_end, output_ports_end, output_port_map, item.second->node());

				    port_id = output_ports_end;
			    });

#if 0
			std::ranges::for_each(r_graph.nodes(),
			                      [&connections  = m_connections,
			                       &ports_to_node = std::as_const(m_ports_to_node),
			                       &graph        = std::as_const(r_graph)](auto const& item) {

									   std::ranges::for_each(item.second.inputs(), [&node = item.second](auto source){
											connections.connect(ports[node][k], ports[source.processor()][source.port()]);
									});
//									  add_connections(item.first, ports_to_node, connections);
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
		std::map<FilterGraph::ProcessorNode const*, std::vector<uint64_t>> m_input_port_map;
		std::map<FilterGraph::ProcessorNode const*, std::vector<uint64_t>> m_output_port_map;

		Canvas m_canvas;
		std::map<FilterGraph::NodeId, Canvas::WidgetHandle<NodeEditor>> m_node_editors;

		template<class PortCollection>
		static uint64_t add_ports_from(PortCollection const& ports,
		                               uint64_t port_id_start,
		                               DynamicMesh<uint64_t, Ui::ToplevelCoordinates>& connections)
		{
			std::ranges::for_each(
			    ports, [&connections, &port_id = port_id_start](auto const& port) {
				    connections.insert(std::make_pair(port_id, port.inputField().location()));
				    ++port_id;
			    });

			return port_id_start;
		}

		static void map_node_to_ports(
		    uint64_t first_port,
		    uint64_t after_last_port,
		    std::map<FilterGraph::ProcessorNode const*, std::vector<uint64_t>>& ports,
		    std::reference_wrapper<FilterGraph::ProcessorNode const> node)
		{
			std::vector<uint64_t> tmp;
			while(first_port != after_last_port)
			{
				tmp.push_back(first_port);
				++first_port;
			}
			ports.insert(std::make_pair(&node.get(), std::move(tmp)));
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