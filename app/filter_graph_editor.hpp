//@	{
//@	"targets":[{"name":"filter_graph_editor.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPHEDITOR_HPP
#define TEXPAINTER_FILTERGRAPHEDITOR_HPP

#include "ui/widget_canvas.hpp"
#include "filtergraph/graph.hpp"

namespace Texpainter
{
	class FilterGraphEditor
	{
	public:
		FilterGraphEditor(Ui::Container& owner, FilterGraph::Graph& graph):
		 r_graph{graph}
		,m_canvas{owner}
		{}

	private:
		FilterGraph::Graph& r_graph;
		Ui::WidgetCanvas m_canvas;
	};
}

#endif