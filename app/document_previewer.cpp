//@	{
//@	 "targets":[{"name":"document_previewer.o", "type":"object"}]
//@	}

#include "./document_previewer.hpp"

void Texpainter::App::DocumentPreviewer::refreshNodeSelector()
{
	m_node_selector.clear();
	m_index_to_node.clear();

	auto const& nodes    = m_doc.get().compositor().nodesWithId();
	auto index_to_select = static_cast<size_t>(-1);
	std::ranges::for_each(
	    nodes,
	    [&node_selector = m_node_selector,
	     node_selected  = m_node_selected,
	     &index_to_select,
	     &index_to_node = m_index_to_node,
	     index          = static_cast<size_t>(0)](auto const& item) mutable {
		    node_selector.append(
		        std::to_string(item.first.value()).append(" ").append(item.second.name()).c_str());
		    if(&item.second == node_selected) { index_to_select = index; }
		    index_to_node.push_back(item.second);
		    ++index;
	    });

	if(index_to_select != static_cast<size_t>(-1))
	{ m_node_selector.selected(static_cast<int>(index_to_select)); }
	else
	{
		m_node_selected = nullptr;
	}
}
