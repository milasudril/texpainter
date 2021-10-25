//@	{
//@	 "targets":[{"name":"compositor.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"compositor.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_APP_COMPOSITOR_HPP
#define TEXPAINTER_APP_COMPOSITOR_HPP

#include "./node_editor.hpp"
#include "./image_processor_selector.hpp"

#include "model/document.hpp"
#include "filtergraph/connection.hpp"
#include "imgproc/image_processor_registry.hpp"
#include "ui/widget_canvas.hpp"
#include "ui/line_segment_renderer.hpp"
#include "ui/menu.hpp"
#include "ui/menu_item.hpp"
#include "ui/error_message_dialog.hpp"
#include "ui/text_entry.hpp"
#include "ui/labeled_input.hpp"
#include "ui/keyboard_state.hpp"
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
			if(item) { r_connectors.node(*m_iter) = item->location() + m_offset; }
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


	class Compositor
	{
		using Canvas           = Ui::WidgetCanvas<FilterGraph::NodeId>;
		using NodeWidget       = NodeEditor<Compositor>;
		using NodeNameInputDlg = Ui::Dialog<
		    InheritFrom<std::pair<FilterGraph::NodeId, Model::CompositorProxy<Model::Document>>,
		                Ui::LabeledInput<Ui::TextEntry>>>;
		using ImageProcessorSelectorDlg = Ui::Dialog<ImageProcessorSelector>;
		using NodeRngSeedDlg            = Ui::Dialog<
            InheritFrom<std::pair<std::reference_wrapper<FilterGraph::Node>, DefaultRng::SeedValue>,
                        Ui::LabeledInput<Ui::TextEntry>>>;
		using NodeUserNameDlg = Ui::Dialog<InheritFrom<std::reference_wrapper<FilterGraph::Node>,
		                                               Ui::LabeledInput<Ui::TextEntry>>>;

	public:
		enum class ControlId : int
		{
			NodeWidgets,
			CopyNode,
			DeleteNode,
			FilterMenu,
			SetRngSeedNode,
			SetNodeName
		};

		Compositor(Compositor&&) = delete;

		Compositor& operator=(Compositor&&) = delete;

		Compositor(Ui::Container& owner, Model::Document& doc);

		Compositor& insert(std::unique_ptr<FilterGraph::AbstractImageProcessor> node,
		                   Ui::WidgetCoordinates loc = Ui::WidgetCoordinates{50.0, 50.0});

		template<auto id, class EventHandler>
		Compositor& eventHandler(EventHandler& eh)
		{
			r_eh       = &eh;
			r_callback = [](void* eh, Compositor& src) {
				auto self = reinterpret_cast<EventHandler*>(eh);
				self->template onUpdated<id>(src);
			};
			return *this;
		}

		template<ControlId>
		void onMouseDown(Canvas& src,
		                 Ui::WidgetCoordinates,
		                 Ui::ScreenCoordinates,
		                 int button,
		                 FilterGraph::NodeId);

		void onKeyDown(Ui::Scancode scancode, Ui::KeyboardState const& state)
		{
			if(isShiftPressed(state) && scancode == Ui::Scancodes::A
			   && state.numberOfPressedKeys() == 2)
			{
				m_filtermenu =
				    std::make_unique<ImageProcessorSelectorDlg>(r_owner, "Select image processor");
				m_filtermenu->eventHandler<ControlId::FilterMenu>(*this);
			}
		}

		void refresh();

		template<ControlId>
		void onRealized(Canvas& src);

		template<ControlId>
		void onMove(Canvas& src, Ui::WidgetCoordinates, FilterGraph::NodeId);

		template<ControlId>
		void onMouseMove(Canvas& src,
		                 Ui::WidgetCoordinates loc_window,
		                 Ui::ToplevelCoordinates loc);

		template<ControlId>
		void onMouseDown(Canvas& src, Ui::WidgetCoordinates, Ui::ScreenCoordinates, int button);

		template<ControlId>
		void onActivated(Ui::MenuItem& src);

		template<ControlId>
		void onViewportMoved(Canvas& src);

		template<ControlId>
		void onCopyCompleted(FilterGraph::NodeId, FilterGraph::Graph::NodeItem);

		template<ControlId>
		void requestItemName(FilterGraph::NodeId, Model::CompositorProxy<Model::Document>);

		void onClicked(NodeWidget const& src, FilterGraph::InputPortIndex port);

		void onClicked(NodeWidget const& src, FilterGraph::OutputPortIndex port);

		template<class PortType>
		void onCompleted(NodeWidget const& src, PortType)
		{
			m_ports.updateLocation(
			    src.node(), src.inputs(), src.outputs(), m_canvas.viewportOffset());
			++m_num_completed_nodes;
			if(m_num_completed_nodes == m_node_editors.size() && m_being_initialized)
			{
				m_linesegs->lineSegments(resolveLineSegs(m_ports.connectors()));
				m_being_initialized = false;
			}
		}

		void handleException(char const* msg, NodeWidget const&, FilterGraph::InputPortIndex)
		{
			// TODO: Use NodeWidget and InputPort to format a better message
			m_err_disp.show(r_owner.get(), "Texpainter: Compositor", msg);
			m_con_proc.reset();
		}

		void handleException(char const* msg, NodeWidget const&, FilterGraph::OutputPortIndex)
		{
			// TODO: Use NodeWidget and OutputPortIndex to format a better message
			m_err_disp.show(r_owner.get(), "Texpainter: Compositor", msg);
			m_con_proc.reset();
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
			m_err_disp.show(r_owner.get(), "Texpainter: Compositor", msg);
		}

		template<ControlId>
		void confirmPositive(NodeNameInputDlg&);

		template<ControlId>
		void dismiss(NodeNameInputDlg&)
		{
			m_copy_name.reset();
		}

		template<ControlId>
		void confirmPositive(NodeRngSeedDlg& src)
		{
			src.widget().first.get().rngSeed(
			    create(Enum::Empty<DefaultRng::SeedValue>{}, src.widget().inputField().content()));
			m_node_set_rng_seed_dlg.reset();
			r_callback(r_eh, *this);
		}

		template<ControlId>
		void dismiss(NodeRngSeedDlg&)
		{
			m_node_set_rng_seed_dlg.reset();
		}

		template<ControlId>
		void confirmPositive(NodeUserNameDlg& src)
		{
			auto& node = src.widget().get();
			node.userName(src.widget().inputField().content());
			m_node_set_name_dlg.reset();
			m_node_editors.find(node.nodeId())->second->update();
		}

		template<ControlId>
		void dismiss(NodeUserNameDlg&)
		{
			m_node_set_name_dlg.reset();
		}

		template<ControlId>
		void confirmPositive(ImageProcessorSelectorDlg& src)
		{
			if(auto name = src.widget().value(); name != nullptr)
			{
				insert(ImageProcessorRegistry::createImageProcessor(name), m_filtermenuloc);
				m_filtermenu.reset();
				r_callback(r_eh, *this);
			}
		}

		template<ControlId>
		void dismiss(ImageProcessorSelectorDlg&)
		{
			m_filtermenu.reset();
		}

		void insertNodeEditor(FilterGraph::Graph::NodeItem item);

		template<ControlId>
		void onActivated(Ui::Listbox& src, int index)
		{
			if(auto name = src.get(index); name != nullptr)
			{
				insert(ImageProcessorRegistry::createImageProcessor(name), m_filtermenuloc);
				m_filtermenu.reset();
				r_callback(r_eh, *this);
			}
		}

	private:
		std::reference_wrapper<Model::Document> m_doc;
		void* r_eh;
		void (*r_callback)(void*, Compositor&);

		PortMap m_ports;
		FilterGraph::NodeId m_sel_node;

		Canvas m_canvas;
		std::unique_ptr<Ui::LineSegmentRenderer> m_linesegs;
		std::map<FilterGraph::NodeId, Canvas::WidgetHandle<NodeWidget>> m_node_editors;
		std::unique_ptr<ImageProcessorSelectorDlg> m_filtermenu;
		Ui::WidgetCoordinates m_filtermenuloc;
		Ui::Menu m_node_menu;
		Ui::MenuItem m_node_copy;
		Ui::MenuItem m_node_delete;
		Ui::MenuItem m_node_set_rng_seed;
		Ui::MenuItem m_node_set_name;

		std::unique_ptr<NodeRngSeedDlg> m_node_set_rng_seed_dlg;
		std::unique_ptr<NodeUserNameDlg> m_node_set_name_dlg;

		std::unique_ptr<FilterGraph::Connection> m_con_proc;
		Ui::ErrorMessageDialog m_err_disp;
		std::reference_wrapper<Ui::Container> r_owner;

		std::unique_ptr<NodeNameInputDlg> m_copy_name;
		size_t m_num_completed_nodes{};
		bool m_being_initialized{true};

		void completeConnection()
		{
			validate(*m_con_proc, *this);
			m_con_proc.reset();
		}
	};

	template<>
	inline void Compositor::onMouseDown<Compositor::ControlId::NodeWidgets>(
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
	inline void Compositor::onMouseMove<Compositor::ControlId::NodeWidgets>(
	    Canvas& src, Ui::WidgetCoordinates loc_window, Ui::ToplevelCoordinates loc)
	{
		m_ports.moveDummyConnectors(loc + src.viewportOffset());
		m_linesegs->lineSegments(resolveLineSegs(m_ports.connectors()));
		m_filtermenuloc = loc_window;
	}

	template<>
	inline void Compositor::onCopyCompleted<Compositor::ControlId::CopyNode>(
	    FilterGraph::NodeId, FilterGraph::Graph::NodeItem item)
	{
		insertNodeEditor(item);
	}

	template<>
	inline void Compositor::onActivated<Compositor::ControlId::CopyNode>(Ui::MenuItem&)
	{
		m_doc.get().compositor().copy<Compositor::ControlId::CopyNode>(*this, m_sel_node);
	}

	template<>
	inline void Compositor::onActivated<Compositor::ControlId::DeleteNode>(Ui::MenuItem&)
	{
		auto node = std::as_const(m_doc.get()).compositor().node(m_sel_node);
		m_ports.removePorts(*node);
		m_node_editors.erase(m_sel_node);
		m_doc.get().compositor().erase(m_sel_node);
		m_linesegs->lineSegments(resolveLineSegs(m_ports.connectors()));
		r_callback(r_eh, *this);
	}

	template<>
	inline void Compositor::onActivated<Compositor::ControlId::SetRngSeedNode>(Ui::MenuItem&)
	{
		auto& node = m_node_editors.find(m_sel_node)->second->node();
		m_node_set_rng_seed_dlg =
		    std::make_unique<NodeRngSeedDlg>(std::pair{std::ref(node), node.rngSeed()},
		                                     r_owner,
		                                     "Set rng seed",
		                                     Texpainter::Ui::Box::Orientation::Horizontal,
		                                     "Value: ");
		m_node_set_rng_seed_dlg->widget()
		    .inputField()
		    .content(toString(node.rngSeed()).c_str())
		    .width(32);
		m_node_set_rng_seed_dlg->eventHandler<Compositor::ControlId::SetRngSeedNode>(*this);
	}

	template<>
	inline void Compositor::onActivated<Compositor::ControlId::SetNodeName>(Ui::MenuItem&)
	{
		auto& node = m_node_editors.find(m_sel_node)->second->node();
		m_node_set_name_dlg =
		    std::make_unique<NodeUserNameDlg>(std::ref(node),
		                                      r_owner,
		                                      "Set node name",
		                                      Texpainter::Ui::Box::Orientation::Horizontal,
		                                      "New name: ");
		m_node_set_name_dlg->widget().inputField().content(node.name());
		m_node_set_name_dlg->eventHandler<Compositor::ControlId::SetNodeName>(*this);
	}

	template<>
	inline void Compositor::onRealized<Compositor::ControlId::NodeWidgets>(Canvas&)
	{
		updateLocations();
	}

	template<>
	inline void Compositor::onMouseDown<Compositor::ControlId::NodeWidgets>(
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
				m_filtermenu =
				    std::make_unique<ImageProcessorSelectorDlg>(r_owner, "Select image processor");
				m_filtermenu->eventHandler<ControlId::FilterMenu>(*this)
				    .widget()
				    .eventHandler<ControlId::FilterMenu>(*this);
				break;
		}
	}

	template<>
	inline void Compositor::onViewportMoved<Compositor::ControlId::NodeWidgets>(Canvas& src)
	{
		auto offset = src.viewportOffset();
		m_linesegs->renderOffset(offset);
	}

	template<>
	inline void Compositor::requestItemName<Compositor::ControlId::CopyNode>(
	    FilterGraph::NodeId node_id, Model::CompositorProxy<Model::Document> compositor)
	{
		m_copy_name = std::make_unique<NodeNameInputDlg>(std::make_pair(node_id, compositor),
		                                                 r_owner,
		                                                 "Copy node",
		                                                 Texpainter::Ui::Box::Orientation::Horizontal,
		                                                 "New name: ");
		m_copy_name->eventHandler<ControlId::CopyNode>(*this);
	}

	template<>
	inline void Compositor::confirmPositive<Compositor::ControlId::CopyNode>(NodeNameInputDlg& src)
	{
		auto& widget = src.widget();
		widget.second.insertNodeWithName<Compositor::ControlId::CopyNode>(
		    *this, widget.first, Model::ItemName{widget.inputField().content()});
		m_copy_name.reset();
		r_callback(r_eh, *this);
	}

	template<>
	inline void Compositor::handleException<Compositor::ControlId::SetRngSeedNode>(
	    char const* msg, Compositor::NodeRngSeedDlg& src)
	{
		m_err_disp.show(r_owner.get(), "Texpainter: Compositor", msg);
		src.widget().inputField().content(toString(src.widget().second).c_str());
		src.widget().first.get().rngSeed(
		    create(Enum::Empty<DefaultRng::SeedValue>{}, src.widget().inputField().content()));
	}
}

#endif