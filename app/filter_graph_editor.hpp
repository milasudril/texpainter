//@	{
//@	"targets":[{"name":"filter_graph_editor.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPHEDITOR_HPP
#define TEXPAINTER_FILTERGRAPHEDITOR_HPP

#include "filtergraph/graph.hpp"
#include "ui/widget_canvas.hpp"
#include "ui/label.hpp"


namespace Texpainter
{
	class FilterGraphEditor
	{
		using NodeEditor = Ui::Label;

	public:
		FilterGraphEditor(Ui::Container& owner, FilterGraph::Graph& graph)
		    : r_graph{graph}
		    , m_canvas{owner}
		{
			std::ranges::transform(
			    r_graph.nodes(),
			    std::inserter(m_node_editors, std::end(m_node_editors)),
			    [&canvas = m_canvas](auto const& node) {
				    auto label = std::to_string(node.first.value());
				    return std::make_pair(
				        node.first, canvas.insert<NodeEditor>(vec2_t{50.0, 50.0}, label.c_str()));
			    });
		}

	private:
		FilterGraph::Graph& r_graph;
		Ui::WidgetCanvas m_canvas;
		std::map<FilterGraph::NodeId, Ui::WidgetCanvas::WidgetHandle<NodeEditor>> m_node_editors;
	};
}

#endif