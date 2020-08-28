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
}

void Texpainter::PortMap::addPorts(Texpainter::FilterGraph::Node& node)
{
	auto input_port_ids = gen_ports_ids(m_current_port_id, node.inputPorts().size());
	m_current_port_id += input_port_ids.size();

	auto output_port_ids = gen_ports_ids(m_current_port_id, node.outputPorts().size());
	m_current_port_id += output_port_ids.size();

	auto insert_connector = [&connectors = m_connectors](auto port_id) {
		connectors.insert(std::pair{port_id, Texpainter::Ui::ToplevelCoordinates{0.0, 0.0}});
	};

	std::ranges::for_each(input_port_ids, insert_connector);
	std::ranges::for_each(output_port_ids, insert_connector);

	m_input_port_map.insert(std::pair{&node, std::move(input_port_ids)});
	m_output_port_map.insert(std::pair{&node, std::move(output_port_ids)});
}

void Texpainter::PortMap::addConnections(Texpainter::FilterGraph::Node const& node)
{
	std::ranges::for_each(
	    node.inputs(),
	    [&inputs     = m_input_port_map.find(&node)->second,
	     &outputs    = m_output_port_map,
	     &connectors = m_connectors,
	     k           = static_cast<size_t>(0)](auto source) mutable {
		    if(auto port = outputs.find(&source.processor()); port != std::end(outputs))
		    { connectors.connect(inputs[k], (port->second)[source.port().value()]); }
		    ++k;
	    });
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

	std::ranges::for_each(
	    r_graph.nodes(), [&ports = m_ports](auto& node_item) { ports.addPorts(node_item.second); });
	std::ranges::for_each(r_graph.nodes(), [&ports = m_ports](auto const& node_item) {
		ports.addConnections(node_item.second);
	});
}

Texpainter::FilterGraphEditor& Texpainter::FilterGraphEditor::insert(
    std::unique_ptr<FilterGraph::AbstractImageProcessor> node)
{
	auto node_item = r_graph.insert(std::move(node));

	auto ip = m_node_editors.insert(MakeNodeEditor{m_canvas}(node_item));
	ip.first->second->eventHandler(*this);

	m_ports.addPorts(node_item.second.get());

	m_canvas.showWidgets();

	return *this;
}

void Texpainter::FilterGraphEditor::updateLocations()
{
	std::ranges::for_each(m_node_editors, [&ports = m_ports](auto const& item) {
		ports.updateLocation(item.second->node(), item.second->inputs(), item.second->outputs());
	});

	m_linesegs->lineSegments(resolveLineSegs(m_ports.m_connectors));
}

template<>
void Texpainter::FilterGraphEditor::onMove<Texpainter::FilterGraphEditor::ControlId::NodeWidgets>(
    Canvas& src, Ui::WidgetCoordinates loc, FilterGraph::NodeId id)
{
	auto node_edit_iter = m_node_editors.find(id);
	assert(node_edit_iter != std::end(m_node_editors));

	auto const& editor = *(node_edit_iter->second);
	m_ports.updateLocation(editor.node(), editor.inputs(), editor.outputs());

	m_linesegs->lineSegments(resolveLineSegs(m_ports.m_connectors));
}