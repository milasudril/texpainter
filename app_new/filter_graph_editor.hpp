//@	{
//@	 "targets":[{"name":"filter_graph_editor.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"filter_graph_editor.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_APP_FILTERGRAPHEDITOR_HPP
#define TEXPAINTER_APP_FILTERGRAPHEDITOR_HPP

#include "./node_editor.hpp"
#include "./node_manager.hpp"

#include "model_new/document.hpp"
#include "filtergraph/connection.hpp"
#include "imgproc/image_processor_registry.hpp"
#include "ui/widget_canvas.hpp"
#include "ui/line_segment_renderer.hpp"
#include "ui/menu.hpp"
#include "ui/menu_item.hpp"
#include "ui/listbox.hpp"
#include "ui/error_message_dialog.hpp"
#include "ui/text_entry.hpp"
#include "ui/labeled_input.hpp"
#include "utils/edge_list.hpp"
#include "utils/inherit_from.hpp"

#include <limits>
#include <cassert>

namespace Texpainter::App
{
	template<auto Tag>
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

	template<auto Tag>
	constexpr PortId<Tag> operator+(PortId<Tag> a, uint64_t offset)
	{
		return a += offset;
	}

	using InputPortId       = PortId<FilterGraph::PortDirection::Input>;
	using OutputPortIndexId = PortId<FilterGraph::PortDirection::Output>;

	template<class Mesh, class IdArrayIterator>
	class ConnectorMove
	{
	public:
		explicit ConnectorMove(Mesh& connectors, IdArrayIterator iter, vec2_t offset)
		    : r_connectors{connectors}
		    , m_iter{iter}
		    , m_offset{offset}
		{
		}

		void operator()(auto const& item)
		{
			r_connectors.node(*m_iter) = item.location() + m_offset;
			++m_iter;
		}

	private:
		Mesh& r_connectors;
		IdArrayIterator m_iter;
		vec2_t m_offset;
	};

	class PortMap
	{
	public:
		PortMap()
		{
			// Dummy connectors for use with mouse cursor tracking
			m_connectors.insert(
			    std::pair{m_port_id_in, Texpainter::Ui::ToplevelCoordinates{0.0, 0.0}});
			++m_port_id_in;
			m_connectors.insert(
			    std::pair{m_port_id_out, Texpainter::Ui::ToplevelCoordinates{0.0, 0.0}});
			++m_port_id_out;
		}

		void addPorts(FilterGraph::Node& node);
		void addConnections(FilterGraph::Node const& node);

		void addConnection(FilterGraph::Endpoint<FilterGraph::InputPortIndex> const& in,
		                   FilterGraph::Endpoint<FilterGraph::OutputPortIndex> const& out)
		{
			m_connectors.connect(m_input_port_map.find(&in.node())->second[in.port().value()],
			                     m_output_port_map.find(&out.node())->second[out.port().value()]);
		}


		void addDummyConnection(FilterGraph::Endpoint<FilterGraph::InputPortIndex> const& in)
		{
			m_connectors.connect(m_input_port_map.find(&in.node())->second[in.port().value()],
			                     OutputPortIndexId{0});
		}

		void addDummyConnection(FilterGraph::Endpoint<FilterGraph::OutputPortIndex> const& out)
		{
			m_connectors.connect(InputPortId{0},
			                     m_output_port_map.find(&out.node())->second[out.port().value()]);
		}

		void removeDummyConnection(FilterGraph::Endpoint<FilterGraph::InputPortIndex> const& in)
		{
			if(auto i = m_input_port_map.find(&in.node()); i != std::end(m_input_port_map))
			{ m_connectors.disconnect(i->second[in.port().value()], OutputPortIndexId{0}); }
		}

		void removeDummyConnection(FilterGraph::Endpoint<FilterGraph::OutputPortIndex> const& out)
		{
			if(auto i = m_output_port_map.find(&out.node()); i != std::end(m_output_port_map))
			{ m_connectors.disconnect(InputPortId{0}, i->second[out.port().value()]); }
		}

		void moveDummyConnectors(Texpainter::Ui::ToplevelCoordinates loc)
		{
			m_connectors.node(InputPortId{0})       = loc;
			m_connectors.node(OutputPortIndexId{0}) = loc;
		}


		void removePorts(FilterGraph::Node const& node);

		void removeConnection(FilterGraph::Endpoint<FilterGraph::InputPortIndex> const& in,
		                      FilterGraph::Endpoint<FilterGraph::OutputPortIndex> const& out)
		{
			m_connectors.disconnect(m_input_port_map.find(&in.node())->second[in.port().value()],
			                        m_output_port_map.find(&out.node())->second[in.port().value()]);
		}

		void removeConnections(FilterGraph::Endpoint<FilterGraph::InputPortIndex> const& in)
		{
			m_connectors.disconnect(inputPort(in));
		}

		void removeConnection(InputPortId a, OutputPortIndexId b) { m_connectors.disconnect(a, b); }

		template<class InputConnectorList, class OutputConnectorList>
		void updateLocation(FilterGraph::Node const& node,
		                    InputConnectorList const& inputs,
		                    OutputConnectorList const& outputs,
		                    vec2_t offset = vec2_t{0, 0})
		{
			{
				auto i = m_input_port_map.find(&node);
				assert(i != std::end(m_input_port_map));
				std::ranges::for_each(inputs,
				                      ConnectorMove{m_connectors, i->second.begin(), offset});
			}

			{
				auto o = m_output_port_map.find(&node);
				assert(o != std::end(m_output_port_map));
				std::ranges::for_each(outputs,
				                      ConnectorMove{m_connectors, o->second.begin(), offset});
			}
		}

		auto const& connectors() const { return m_connectors; }

		auto const& outputPorts(FilterGraph::Node const& node) const
		{
			return m_output_port_map.find(&node)->second;
		}

		InputPortId inputPort(FilterGraph::Endpoint<FilterGraph::InputPortIndex> const& sink) const
		{
			return m_input_port_map.find(&sink.node())->second[sink.port().value()];
		}

	private:
		InputPortId m_port_id_in{0};
		OutputPortIndexId m_port_id_out{0};

		EdgeList<InputPortId, OutputPortIndexId, Ui::ToplevelCoordinates> m_connectors;
		std::map<FilterGraph::Node const*, std::vector<InputPortId>> m_input_port_map;
		std::map<FilterGraph::Node const*, std::vector<OutputPortIndexId>> m_output_port_map;
	};


	class FilterGraphEditor
	{
		using Canvas           = Ui::WidgetCanvas<FilterGraph::NodeId>;
		using NodeWidget       = NodeEditor<FilterGraphEditor>;
		using NodeNameInputDlg = Texpainter::Ui::Dialog<
		    InheritFrom<std::pair<FilterGraph::NodeId, Model::CompositorProxy<Model::Document>>,
		                Ui::LabeledInput<Ui::TextEntry>>>;

	public:
		enum class ControlId : int
		{
			NodeWidgets,
			CopyNode,
			DeleteNode,
			FilterMenu
		};

		FilterGraphEditor(FilterGraphEditor&&) = delete;

		FilterGraphEditor& operator=(FilterGraphEditor&&) = delete;

		FilterGraphEditor(Ui::Container& owner,
		                  Model::Document& doc,
		                  std::map<FilterGraph::NodeId, vec2_t> const& node_locations);

		FilterGraphEditor& insert(std::unique_ptr<FilterGraph::AbstractImageProcessor> node,
		                          Ui::WidgetCoordinates loc = Ui::WidgetCoordinates{50.0, 50.0});

		template<auto id, class EventHandler>
		FilterGraphEditor& eventHandler(EventHandler& eh)
		{
			r_eh       = &eh;
			r_callback = [](void* eh, FilterGraphEditor& src) {
				auto self = reinterpret_cast<EventHandler*>(eh);
				self->template graphUpdated<id>(src);
			};
			return *this;
		}

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
		void onMouseMove(Canvas& src, Ui::ToplevelCoordinates loc);

		template<ControlId>
		void onMouseDown(Canvas& src, Ui::WidgetCoordinates, Ui::ScreenCoordinates, int button);

		template<ControlId>
		void onActivated(Ui::MenuItem& src);

		template<ControlId>
		void onActivated(Ui::Listbox& src, int item);

		template<ControlId>
		void onViewportMoved(Canvas& src);

		template<ControlId>
		void onCopyCompleted(FilterGraph::NodeId, FilterGraph::Graph::NodeItem);

		template<ControlId>
		void requestItemName(FilterGraph::NodeId, Model::CompositorProxy<Model::Document>);

		void onClicked(NodeWidget const& src, FilterGraph::InputPortIndex port);

		void onClicked(NodeWidget const& src, FilterGraph::OutputPortIndex port);

		void handleException(char const* msg, NodeWidget const&, FilterGraph::InputPortIndex)
		{
			// TODO: Use NodeWidget and InputPort to format a better message
			m_err_disp.show(r_owner.get(), "Texpainter", msg);
		}

		void handleException(char const* msg, NodeWidget const&, FilterGraph::OutputPortIndex)
		{
			// TODO: Use NodeWidget and OutputPortIndex to format a better message
			m_err_disp.show(r_owner.get(), "Texpainter", msg);
		}

		void onChanged(NodeWidget const& src,
		               FilterGraph::ParamName name,
		               FilterGraph::ParamValue value)
		{
			src.node().set(name, value);
			r_callback(r_eh, *this);
		}

		void notCompleted(FilterGraph::Connection const& conn)
		{
			m_ports.removeDummyConnection(conn.source());
			m_ports.removeDummyConnection(conn.sink());
		}

		void selfConnection(FilterGraph::Connection const& conn)
		{
			m_ports.removeDummyConnection(conn.source());
			m_ports.removeDummyConnection(conn.sink());
		}

		void typeMismatch(FilterGraph::Connection const& conn)
		{
			m_ports.removeDummyConnection(conn.source());
			m_ports.removeDummyConnection(conn.sink());
		}

		void connectionOk(FilterGraph::Connection const& conn)
		{
			m_ports.removeConnections(conn.sink());
			m_ports.removeDummyConnection(conn.source());
			establish(conn);
			m_doc.get().compositor().clearValidationState();
			m_ports.addConnection(conn.sink(), conn.source());
			m_linesegs->lineSegments(resolveLineSegs(m_ports.connectors()));
			r_callback(r_eh, *this);
		}

		void updateLocations();

		std::map<FilterGraph::NodeId, vec2_t> nodeLocations() const;

		template<auto id, class Src>
		void handleException(char const* msg, Src&)
		{
			m_err_disp.show(r_owner.get(), "Texpainter", msg);
		}


	private:
		std::reference_wrapper<Model::Document> m_doc;
		void* r_eh;
		void (*r_callback)(void*, FilterGraphEditor&);

		PortMap m_ports;

		FilterGraph::NodeId m_sel_node;

		Canvas m_canvas;
		std::unique_ptr<Ui::LineSegmentRenderer> m_linesegs;
		std::map<FilterGraph::NodeId, Canvas::WidgetHandle<NodeWidget>> m_node_editors;
		Canvas::WidgetHandle<Ui::Listbox> m_filtermenu;
		Ui::WidgetCoordinates m_filtermenuloc;
		Ui::Menu m_node_menu;
		Ui::MenuItem m_node_copy;
		Ui::MenuItem m_node_delete;

		std::unique_ptr<FilterGraph::Connection> m_con_proc;
		Ui::ErrorMessageDialog m_err_disp;
		std::reference_wrapper<Ui::Container> r_owner;

		std::unique_ptr<NodeNameInputDlg> m_copy_name;


		void completeConnection()
		{
			validate(*m_con_proc, *this);
			m_con_proc.reset();
		}
	};

	template<>
	inline void FilterGraphEditor::onMouseDown<FilterGraphEditor::ControlId::NodeWidgets>(
	    Canvas&,
	    Ui::WidgetCoordinates loc,
	    Ui::ScreenCoordinates,
	    int button,
	    FilterGraph::NodeId node)
	{
		if(button == 3)
		{
			if(node != Model::Compositor::OutputNodeId)
			{
				m_sel_node      = node;
				m_filtermenuloc = loc;
				m_node_menu.show().popupAtCursor();
			}
		}
	}

	template<>
	inline void FilterGraphEditor::onMouseMove<FilterGraphEditor::ControlId::NodeWidgets>(
	    Canvas& src, Ui::ToplevelCoordinates loc)
	{
		m_ports.moveDummyConnectors(loc + src.viewportOffset());
		m_linesegs->lineSegments(resolveLineSegs(m_ports.connectors()));
	}

	template<>
	inline void FilterGraphEditor::onCopyCompleted<FilterGraphEditor::ControlId::CopyNode>(
	    FilterGraph::NodeId, FilterGraph::Graph::NodeItem item)
	{
		auto ip = m_node_editors.insert(std::pair{
		    item.first,
		    m_canvas.template insert<NodeWidget>(item.first, m_filtermenuloc, item.second)});
		ip.first->second->eventHandler(*this);
		m_ports.addPorts(item.second.get());
		m_canvas.showWidgets();
	}

	template<>
	inline void FilterGraphEditor::onActivated<FilterGraphEditor::ControlId::CopyNode>(
	    Ui::MenuItem&)
	{
		m_doc.get().compositor().copy<FilterGraphEditor::ControlId::CopyNode>(*this, m_sel_node);
	}

	template<>
	inline void FilterGraphEditor::onActivated<FilterGraphEditor::ControlId::DeleteNode>(
	    Ui::MenuItem&)
	{
		auto node = std::as_const(m_doc.get()).compositor().node(m_sel_node);
		m_ports.removePorts(*node);
		m_node_editors.erase(m_sel_node);
		m_doc.get().compositor().erase(m_sel_node);
		m_linesegs->lineSegments(resolveLineSegs(m_ports.connectors()));
		r_callback(r_eh, *this);
	}

	template<>
	inline void FilterGraphEditor::onRealized<FilterGraphEditor::ControlId::NodeWidgets>(Canvas&)
	{
		updateLocations();
	}

	template<>
	inline void FilterGraphEditor::onMouseDown<FilterGraphEditor::ControlId::NodeWidgets>(
	    Canvas&, Ui::WidgetCoordinates loc, Ui::ScreenCoordinates, int button)
	{
		switch(button)
		{
			case 1:
				if(m_con_proc != nullptr)
				{
					m_ports.removeDummyConnection(m_con_proc->sink());
					m_ports.removeDummyConnection(m_con_proc->source());
					m_con_proc.reset();
					m_linesegs->lineSegments(resolveLineSegs(m_ports.connectors()));
				}
				m_filtermenu.reset();
				break;
			case 3:
				m_filtermenuloc = loc;
				m_filtermenu.reset();  // Reset first so the same id can be reused
				m_filtermenu = m_canvas.insert<Ui::Listbox>(
				    FilterGraph::NodeId{static_cast<uint64_t>(-1)}, loc);
				std::ranges::for_each(ImageProcessorRegistry::processorNames(),
				                      [&menu = *m_filtermenu](auto item) { menu.append(item); });
				m_filtermenu->eventHandler<ControlId::FilterMenu>(*this);
				m_canvas.showWidgets();
				break;
		}
	}

	template<>
	inline void FilterGraphEditor::onActivated<FilterGraphEditor::ControlId::FilterMenu>(
	    Ui::Listbox& box, int item)
	{
		auto name = box.get(item);
		insert(ImageProcessorRegistry::createImageProcessor(name), m_filtermenuloc);
		m_filtermenu.reset();
	}

	template<>
	inline void FilterGraphEditor::onViewportMoved<FilterGraphEditor::ControlId::NodeWidgets>(
	    Canvas& src)
	{
		auto offset = src.viewportOffset();
		m_linesegs->renderOffset(offset);
	}

	template<>
	inline void FilterGraphEditor::requestItemName<FilterGraphEditor::ControlId::CopyNode>(
	    FilterGraph::NodeId node_id, Model::CompositorProxy<Model::Document> compositor)
	{
		m_copy_name = std::make_unique<NodeNameInputDlg>(std::make_pair(node_id, compositor),
		                                                 r_owner,
		                                                 "Copy node",
		                                                 Texpainter::Ui::Box::Orientation::Vertical,
		                                                 "New name");
	}
}

#endif