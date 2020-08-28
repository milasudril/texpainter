//@	{
//@	 "targets":[{"name":"filter_graph_editor.o","type":"object"}]
//@	}

#include "./filter_graph_editor.hpp"
#include "utils/integer_sequence_iterator.hpp"

#include <algorithm>

namespace
{
	template<class Canvas>
	class MakeNodeEditor
	{
		using NodeWidget = Texpainter::NodeEditor<Texpainter::FilterGraphEditor>;

	public:
		explicit MakeNodeEditor(Canvas& canvas): r_canvas{canvas} {}

		auto operator()(auto& item) const
		{
			auto tmp = r_canvas.template insert<NodeWidget>(item.first, m_insert_loc, item.second);
			return std::pair{item.first, std::move(tmp)};
		}

	private:
		// TODO: Do not hard-code insert position?
		Texpainter::Ui::WidgetCoordinates m_insert_loc{50.0, 50.0};
		Canvas& r_canvas;
	};

	std::vector<Texpainter::PortId> gen_ports_ids(Texpainter::PortId initial_value, size_t n)
	{
		std::vector<Texpainter::PortId> ret;
		ret.reserve(n);
		std::copy_n(Texpainter::IntegerSequenceIterator{initial_value}, n, std::back_inserter(ret));
		return ret;
	}

	template<class PortCollection, class InputPortMap, class OutputPortMap>
	Texpainter::PortId insert_ports(Texpainter::FilterGraph::Node& node,
	                                Texpainter::PortId current_port_id,
	                                PortCollection& connectors,
	                                InputPortMap& input_ports,
	                                OutputPortMap& output_ports)
	{
		auto input_port_ids = gen_ports_ids(current_port_id, node.inputPorts().size());
		current_port_id += input_port_ids.size();

		auto output_port_ids = gen_ports_ids(current_port_id, node.outputPorts().size());
		current_port_id += output_port_ids.size();

		auto insert_connector = [&connectors](auto port_id) {
			connectors.insert(std::pair{port_id, Texpainter::Ui::ToplevelCoordinates{0.0, 0.0}});
		};

		std::ranges::for_each(input_port_ids, insert_connector);
		std::ranges::for_each(output_port_ids, insert_connector);

		input_ports.insert(std::pair{&node, std::move(input_port_ids)});
		output_ports.insert(std::pair{&node, std::move(output_port_ids)});

		return current_port_id;
	}
}

Texpainter::FilterGraphEditor::FilterGraphEditor(Ui::Container& owner, FilterGraph::Graph& graph)
    : r_graph{graph}
    , m_canvas{owner}
    , m_node_copy{m_node_menu, "Copy"}
    , m_node_delete{m_node_menu, "Delete"}
{
	m_linesegs = m_canvas.insert<Ui::LineSegmentRenderer>();
	std::ranges::transform(r_graph.nodes(),
	                       std::inserter(m_node_editors, std::end(m_node_editors)),
	                       MakeNodeEditor{m_canvas});

	m_canvas.eventHandler<ControlId::NodeWidgets>(*this);
	std::ranges::for_each(m_node_editors, [this](auto& item) { item.second->eventHandler(*this); });
	m_node_copy.eventHandler<ControlId::CopyNode>(*this);
	m_node_delete.eventHandler<ControlId::DeleteNode>(*this);

	//	std::ranges::for_each(r_graph.nodes(), insert_ports);
}

Texpainter::FilterGraphEditor& Texpainter::FilterGraphEditor::insert(
    std::unique_ptr<FilterGraph::AbstractImageProcessor> node)
{
	auto node_item = r_graph.insert(std::move(node));

	auto ip = m_node_editors.insert(MakeNodeEditor{m_canvas}(node_item));
	ip.first->second->eventHandler(*this);

	m_ports.m_current_port_id = insert_ports(node_item.second.get(),
	                                         m_ports.m_current_port_id,
	                                         m_ports.m_connectors,
	                                         m_ports.m_input_port_map,
	                                         m_ports.m_output_port_map);

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
	    [&port_id         = m_ports.m_current_port_id,
	     &connections     = m_ports.m_connectors,
	     &input_port_map  = m_ports.m_input_port_map,
	     &output_port_map = m_ports.m_output_port_map](auto const& item) {
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

	std::ranges::for_each(
	    r_graph.nodes(),
	    [&connections     = m_ports.m_connectors,
	     &input_port_map  = std::as_const(m_ports.m_input_port_map),
	     &output_port_map = std::as_const(m_ports.m_output_port_map)](auto const& item) {
		    auto i = input_port_map.find(&item.second);
		    assert(i != std::end(input_port_map));
		    std::ranges::for_each(item.second.inputs(),
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
		auto i = m_ports.m_input_port_map.find(&node_edit.node());
		assert(i != std::end(m_ports.m_input_port_map));
		std::ranges::for_each(node_edit.inputs(),
		                      ConnectorMove{m_ports.m_connectors, i->second.begin()});
	}

	{
		auto o = m_ports.m_output_port_map.find(&node_edit.node());
		assert(o != std::end(m_ports.m_output_port_map));
		std::ranges::for_each(node_edit.outputs(),
		                      ConnectorMove{m_ports.m_connectors, o->second.begin()});
	}

	m_linesegs->lineSegments(resolveLineSegs(m_ports.m_connectors));
}