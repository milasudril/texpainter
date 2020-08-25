//@	{
//@	 "targets":[{"name":"filter_graph_editor.o","type":"object"}]
//@	}

#include "./filter_graph_editor.hpp"
#include <algorithm>

Texpainter::FilterGraphEditor::FilterGraphEditor(Ui::Container& owner, FilterGraph::Graph& graph)
    : r_graph{graph}
    , m_current_port_id{0}
    , m_canvas{owner}
    , m_node_copy{m_node_menu, "Copy"}
    , m_node_delete{m_node_menu, "Delete"}
{
	m_linesegs = m_canvas.insert<Ui::LineSegmentRenderer>();
	std::ranges::transform(r_graph.nodes(),
	                       std::inserter(m_node_editors, std::end(m_node_editors)),
	                       [&canvas = m_canvas](auto& node) {
		                       return std::make_pair(
		                           node.first,
		                           canvas.insert<NodeWidget>(
		                               node.first, Ui::WidgetCoordinates{50.0, 50.0}, node.second));
	                       });

	m_canvas.eventHandler<ControlId::NodeWidgets>(*this);
	std::ranges::for_each(m_node_editors, [this](auto& item) { item.second->eventHandler(*this); });
	m_node_copy.eventHandler<ControlId::CopyNode>(*this);
	m_node_delete.eventHandler<ControlId::DeleteNode>(*this);
}

Texpainter::FilterGraphEditor& Texpainter::FilterGraphEditor::insert(
    std::unique_ptr<FilterGraph::AbstractImageProcessor> node)
{
	auto node_item = r_graph.insert(std::move(node));

	// TODO: Do not hard-code insert position?
	auto ip = m_node_editors.insert(
	    std::make_pair(node_item.first,
	                   m_canvas.insert<NodeWidget>(
	                       node_item.first, Ui::WidgetCoordinates{50.0, 50.0}, node_item.second)));
	ip.first->second->eventHandler(*this);

	auto gen_ports_ids = [](auto ports, auto& port_id) {
		std::vector<PortId> ret;
		ret.reserve(ports.size());
		std::generate_n(std::back_inserter(ret), ports.size(), [&port_id]() { return port_id++; });
		return ret;
	};

	auto input_port_ids  = gen_ports_ids(node_item.second.get().inputPorts(), m_current_port_id);
	auto output_port_ids = gen_ports_ids(node_item.second.get().outputPorts(), m_current_port_id);

	auto insert_connector = [&connectors = m_connectors](auto port_id) {
		connectors.insert(std::make_pair(port_id, Ui::ToplevelCoordinates{0.0, 0.0}));
	};

	std::ranges::for_each(input_port_ids, insert_connector);
	std::ranges::for_each(output_port_ids, insert_connector);

	m_input_port_map.insert(std::make_pair(&node_item.second.get(), std::move(input_port_ids)));
	m_output_port_map.insert(std::make_pair(&node_item.second.get(), std::move(output_port_ids)));

	m_canvas.showWidgets();

	return *this;
}

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
	std::ranges::for_each(
	    m_node_editors,
	    [&port_id         = m_current_port_id,
	     &connections     = m_connectors,
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
	                      [&connections     = m_connectors,
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

namespace
{
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
}

template<>
void Texpainter::FilterGraphEditor::onMove<Texpainter::FilterGraphEditor::ControlId::NodeWidgets>(
    Canvas& src, Ui::WidgetCoordinates loc, FilterGraph::NodeId id)
{
	auto node_edit_iter = m_node_editors.find(id);
	assert(node_edit_iter != std::end(m_node_editors));

	auto const& node_edit = *(node_edit_iter->second);

	{
		auto i = m_input_port_map.find(&node_edit.node());
		assert(i != std::end(m_input_port_map));
		std::ranges::for_each(node_edit.inputs(), ConnectorMove{m_connectors, i->second.begin()});
	}

	{
		auto o = m_output_port_map.find(&node_edit.node());
		assert(o != std::end(m_output_port_map));
		std::ranges::for_each(node_edit.outputs(), ConnectorMove{m_connectors, o->second.begin()});
	}

	m_linesegs->lineSegments(resolveLineSegs(m_connectors));
}