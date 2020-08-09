//@	{
//@	"targets":[{"name":"filter_graph_editor.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPHEDITOR_HPP
#define TEXPAINTER_FILTERGRAPHEDITOR_HPP

#include "./node_editor.hpp"

#include "filtergraph/graph.hpp"
#include "ui/widget_canvas.hpp"
#include "utils/dynamic_mesh.hpp"


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
			    [&port_id = m_current_port_id, &connections = m_connections](auto const& node) {
				    add_ports_from(*node.second, port_id, connections);
			    });

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

		static void add_ports_from(NodeEditor const& node_edit,
		                           uint64_t& port_id,
		                           DynamicMesh<uint64_t, Ui::ToplevelCoordinates>& connections)
		{
			add_ports_from(node_edit.inputs(), port_id, connections);
			add_ports_from(node_edit.outputs(), port_id, connections);
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
