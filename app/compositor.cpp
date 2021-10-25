//@	{
//@	 "targets":[{"name":"compositor.o","type":"object"}]
//@	}

#include "./compositor.hpp"
#include "utils/integer_sequence_iterator.hpp"

#include <algorithm>

namespace
{
	template<class PortIdType>
	std::vector<PortIdType> gen_ports_ids(PortIdType initial_value, size_t n)
	{
		std::vector<PortIdType> ret;
		ret.reserve(n);
		std::copy_n(Texpainter::IntegerSequenceIterator{initial_value}, n, std::back_inserter(ret));
		return ret;
	}
}

void Texpainter::App::PortMap::addPorts(Texpainter::FilterGraph::Node& node)
{
	auto input_port_ids = gen_ports_ids(m_port_id_in, node.inputPorts().size());
	m_port_id_in += input_port_ids.size();

	auto output_port_ids = gen_ports_ids(m_port_id_out, node.outputPorts().size());
	m_port_id_out += output_port_ids.size();

	auto insert_connector = [&connectors = m_connectors](auto port_id) {
		connectors.insert(std::pair{port_id, Texpainter::Ui::ToplevelCoordinates{0.0, 0.0}});
	};

	std::ranges::for_each(input_port_ids, insert_connector);
	std::ranges::for_each(output_port_ids, insert_connector);

	m_input_port_map.insert(std::pair{&node, std::move(input_port_ids)});
	m_output_port_map.insert(std::pair{&node, std::move(output_port_ids)});
}

void Texpainter::App::PortMap::removePorts(FilterGraph::Node const& node)
{
	std::ranges::for_each(m_output_port_map.find(&node)->second,
	                      [&connections = m_connectors](auto item) { connections.remove(item); });
	m_output_port_map.erase(&node);

	std::ranges::for_each(m_input_port_map.find(&node)->second,
	                      [&connections = m_connectors](auto item) { connections.remove(item); });
	m_input_port_map.erase(&node);
}

void Texpainter::App::PortMap::addConnections(Texpainter::FilterGraph::Node const& node)
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


namespace
{
	template<class Canvas>
	class MakeNodeEditor
	{
		using NodeWidget = Texpainter::App::NodeEditor<Texpainter::App::Compositor>;

	public:
		explicit MakeNodeEditor(
		    Canvas& canvas,
		    std::map<Texpainter::FilterGraph::NodeId, Texpainter::vec2_t> const& node_locs)
		    : r_node_locs{node_locs}
		    , r_canvas{canvas}
		    , k{0}
		{
		}

		auto operator()(auto& item)
		{
			auto const i   = r_node_locs.find(item.first);
			auto const loc = i != std::end(r_node_locs)
			                     ? Texpainter::vec2_t{i->second}
			                     : Texpainter::vec2_t{20.0 + 200.0 * static_cast<double>(k), 20.0};
			auto tmp = r_canvas.template insert<NodeWidget>(
			    item.first, Texpainter::Ui::WidgetCoordinates{loc}, item.second);
			++k;
			return std::pair{item.first, std::move(tmp)};
		}

	private:
		std::map<Texpainter::FilterGraph::NodeId, Texpainter::vec2_t> const& r_node_locs;
		Canvas& r_canvas;
		size_t k;
	};
}

Texpainter::App::Compositor::Compositor(Ui::Container& owner, Model::Document& doc)
    : m_doc{doc}
    , r_callback{[](void*, Compositor&) {}}
    , m_canvas{owner}
    , m_filtermenuloc{0.0, 0.0}
    , m_node_copy{m_node_menu, "Copy"}
    , m_node_delete{m_node_menu, "Delete"}
    , m_node_set_rng_seed{m_node_menu, "Set random seed"}
    , m_node_set_name{m_node_menu, "Set name"}
    , r_owner{owner}
{
	m_linesegs = m_canvas.insert<Ui::LineSegmentRenderer>();
	std::ranges::transform(m_doc.get().compositor().nodesWithId(),
	                       std::inserter(m_node_editors, std::end(m_node_editors)),
	                       MakeNodeEditor{m_canvas, m_doc.get().nodeLocations()});

	m_canvas.eventHandler<ControlId::NodeWidgets>(*this);
	std::ranges::for_each(m_node_editors, [this](auto& item) { item.second->eventHandler(*this); });
	m_node_copy.eventHandler<ControlId::CopyNode>(*this);
	m_node_delete.eventHandler<ControlId::DeleteNode>(*this);
	m_node_set_rng_seed.eventHandler<ControlId::SetRngSeedNode>(*this);
	m_node_set_name.eventHandler<ControlId::SetNodeName>(*this);

	std::ranges::for_each(
	    m_doc.get().compositor().nodesWithId(),
	    [&ports = m_ports](auto& node_item) { ports.addPorts(node_item.second); });
	std::ranges::for_each(
	    m_doc.get().compositor().nodesWithId(),
	    [&ports = m_ports](auto const& node_item) { ports.addConnections(node_item.second); });
}

std::map<Texpainter::FilterGraph::NodeId, Texpainter::vec2_t> Texpainter::App::Compositor::
    nodeLocations() const
{
	std::map<FilterGraph::NodeId, vec2_t> ret;
	std::ranges::transform(
	    m_doc.get().compositor().nodesWithId(),
	    std::inserter(ret, std::end(ret)),
	    [&canvas = m_canvas](auto const& item) {
		    return std::pair{item.first, canvas.widgetLocation(item.first).value()};
	    });

	return ret;
}

Texpainter::App::Compositor& Texpainter::App::Compositor::insert(
    std::unique_ptr<FilterGraph::AbstractImageProcessor> node, Ui::WidgetCoordinates insert_loc)
{
	auto item = m_doc.get().compositor().insert(std::move(node));

	auto ip = m_node_editors.insert(std::pair{
	    item.first, m_canvas.template insert<NodeWidget>(item.first, insert_loc, item.second)});
	ip.first->second->eventHandler(*this);

	m_ports.addPorts(item.second.get());

	m_canvas.showWidgets();

	return *this;
}

void Texpainter::App::Compositor::updateLocations()
{
	std::ranges::for_each(m_node_editors, [&ports = m_ports](auto const& item) {
		ports.updateLocation(item.second->node(), item.second->inputs(), item.second->outputs());
	});
	m_canvas.updateCanvasSize();
	m_linesegs->lineSegments(resolveLineSegs(m_ports.connectors()));
}

void Texpainter::App::Compositor::onClicked(NodeWidget const& src, FilterGraph::InputPortIndex port)
{
	if(m_con_proc == nullptr)
	{
		m_con_proc = std::make_unique<FilterGraph::Connection>(src.node(), port);
		m_ports.addDummyConnection(m_con_proc->sink());
		m_linesegs->lineSegments(resolveLineSegs(m_ports.connectors()));
	}
	else if(m_con_proc->sink().valid())
	{
		// Ignore new trigger
		return;
	}
	else
	{
		m_con_proc->sink(src.node(), port);
		completeConnection();
	}
}

void Texpainter::App::Compositor::onClicked(NodeWidget const& src,
                                            FilterGraph::OutputPortIndex port)
{
	if(m_con_proc == nullptr)
	{
		m_con_proc = std::make_unique<FilterGraph::Connection>(src.node(), port);
		m_ports.addDummyConnection(m_con_proc->source());
		m_linesegs->lineSegments(resolveLineSegs(m_ports.connectors()));
	}
	else if(m_con_proc->source().valid())
	{
		// Ignore new trigger
		return;
	}
	else
	{
		m_con_proc->source(src.node(), port);
		completeConnection();
	}
}

template<>
void Texpainter::App::Compositor::onMove<Texpainter::App::Compositor::ControlId::NodeWidgets>(
    Canvas& src, Ui::WidgetCoordinates, FilterGraph::NodeId id)
{
	auto node_edit_iter = m_node_editors.find(id);
	if(node_edit_iter == std::end(m_node_editors)) { return; }

	auto const& editor = *(node_edit_iter->second);
	m_ports.updateLocation(editor.node(), editor.inputs(), editor.outputs(), src.viewportOffset());

	m_linesegs->lineSegments(resolveLineSegs(m_ports.connectors()));
}

void Texpainter::App::Compositor::insertNodeEditor(FilterGraph::Graph::NodeItem item)
{
	auto ip = m_node_editors.insert(
	    std::pair{item.first,
	              m_canvas.template insert<NodeWidget>(item.first, m_filtermenuloc, item.second)});
	ip.first->second->eventHandler(*this);
	m_ports.addPorts(item.second.get());
	m_canvas.showWidgets();
	r_callback(r_eh, *this);
}

void Texpainter::App::Compositor::refresh()
{
	m_node_editors.clear();

	std::ranges::transform(m_doc.get().compositor().nodesWithId(),
	                       std::inserter(m_node_editors, std::end(m_node_editors)),
	                       MakeNodeEditor{m_canvas, m_doc.get().nodeLocations()});
	std::ranges::for_each(m_node_editors, [this](auto& item) { item.second->eventHandler(*this); });

	m_ports = PortMap{};
	std::ranges::for_each(
	    m_doc.get().compositor().nodesWithId(),
	    [&ports = m_ports](auto& node_item) { ports.addPorts(node_item.second); });
	std::ranges::for_each(
	    m_doc.get().compositor().nodesWithId(),
	    [&ports = m_ports](auto const& node_item) { ports.addConnections(node_item.second); });

	m_canvas.showWidgets();
	m_linesegs->lineSegments(resolveLineSegs(m_ports.connectors()));
}