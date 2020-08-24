//@	{
//@	 "targets":[{"name":"filter_graph_editor.o","type":"object"}]
//@	}

#include "./filter_graph_editor.hpp"

namespace
{
	template<class PortCollection, class PortId>
	PortId add_ports_from(
	    PortCollection const& ports,
	    PortId port_id_start,
	    Texpainter::DynamicMesh<PortId, Texpainter::Ui::ToplevelCoordinates>& connections)
	{
		std::ranges::for_each(ports, [&connections, &port_id = port_id_start](auto const& port) {
			connections.insert(std::make_pair(port_id, port.location()));
			++port_id;
		});

		return port_id_start;
	}

	template<class PortId>
	void map_node_to_ports(
	    PortId first_port,
	    PortId after_last_port,
	    std::map<Texpainter::FilterGraph::Node const*, std::vector<PortId>>& ports,
	    std::reference_wrapper<Texpainter::FilterGraph::Node const> node)
	{
		std::vector<PortId> tmp;
		while(first_port != after_last_port)
		{
			tmp.push_back(first_port);
			++first_port;
		}
		ports.insert(std::make_pair(&node.get(), std::move(tmp)));
	}

}

void Texpainter::FilterGraphEditor::init()
{
	m_current_port_id = PortId{1};
	std::ranges::for_each(
	    m_node_editors,
	    [&port_id         = m_current_port_id,
	     &connections     = m_connections,
	     &input_port_map  = m_input_port_map,
	     &output_port_map = m_output_port_map](auto const& item) {
		    auto port_id_start = port_id;
		    auto input_ports_end =
		        add_ports_from(item.second->inputs(), port_id_start, connections);
		    map_node_to_ports(port_id_start, input_ports_end, input_port_map, item.second->node());
		    auto output_ports_end =
		        add_ports_from(item.second->outputs(), input_ports_end, connections);
		    map_node_to_ports(
		        input_ports_end, output_ports_end, output_port_map, item.second->node());

		    port_id = output_ports_end;
	    });

	std::ranges::for_each(r_graph.nodes(),
	                      [&connections     = m_connections,
	                       &input_port_map  = std::as_const(m_input_port_map),
	                       &output_port_map = std::as_const(m_output_port_map)](auto const& item) {
		                      auto i = input_port_map.find(&item.second);
		                      assert(i != std::end(input_port_map));
		                      std::ranges::for_each(
		                          item.second.inputs(),
		                          [&connections,
		                           &output_port_map,
		                           &inputs = i->second,
		                           input   = static_cast<size_t>(0)](auto source) mutable {
			                          auto o = output_port_map.find(&source.processor());
			                          if(o != std::end(output_port_map))
			                          {
				                          connections.connect(inputs[input],
				                                              (o->second)[source.port().value()]);
			                          }
			                          ++input;
		                          });
	                      });
}

template<>
void Texpainter::FilterGraphEditor::onMove<Texpainter::FilterGraphEditor::ControlId::NodeWidgets>(
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
		                      if(ids[k].valid()) { connections.moveTo(ids[k], item.location()); }
		                      ++k;
	                      });

	auto o = m_output_port_map.find(&node_edit.node());
	assert(o != std::end(m_output_port_map));
	std::ranges::for_each(node_edit.outputs(),
	                      [&connections = m_connections,
	                       &ids         = o->second,
	                       k            = static_cast<size_t>(0)](auto const& item) mutable {
		                      if(ids[k].valid()) { connections.moveTo(ids[k], item.location()); }
		                      ++k;
	                      });

	m_linesegs->lineSegments(resolveLineSegs(m_connections));
}