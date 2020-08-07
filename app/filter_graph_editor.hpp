//@	{
//@	"targets":[{"name":"filter_graph_editor.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPHEDITOR_HPP
#define TEXPAINTER_FILTERGRAPHEDITOR_HPP

#include "./node_editor.hpp"

#include "filtergraph/graph.hpp"
#include "ui/widget_canvas.hpp"


namespace Texpainter
{
	class FilterGraphEditor
	{
	public:
		FilterGraphEditor(Ui::Container& owner, FilterGraph::Graph& graph)
		    : r_graph{graph}
		    , m_canvas{owner}
		{
			std::ranges::transform(
			    r_graph.nodes(),
			    std::inserter(m_node_editors, std::end(m_node_editors)),
			    [&canvas = m_canvas](auto const& node) {
				    return std::make_pair(
				        node.first,
				        canvas.insert<NodeEditor>(node.first, vec2_t{50.0, 50.0}, node.second));
			    });
		}

	private:
		FilterGraph::Graph& r_graph;
		Ui::WidgetCanvas<FilterGraph::NodeId> m_canvas;
		std::map<FilterGraph::NodeId,
		         Ui::WidgetCanvas<FilterGraph::NodeId>::WidgetHandle<NodeEditor>>
		    m_node_editors;
	};
}

#endif