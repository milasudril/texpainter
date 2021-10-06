//@	{
//@	 "targets":[{"name":"document_previewer.o", "type":"object"}]
//@	}

#include "./document_previewer.hpp"

void Texpainter::App::DocumentPreviewer::refreshNodeSelector()
{
	m_node_selector.clear();

	auto const& nodes = m_doc.get().compositor().nodesWithId();
	std::ranges::for_each(nodes, [&node_selector = m_node_selector](auto const& item){
		node_selector.append(std::to_string(item.first.value()).append(" ").append(item.second.name()).c_str());
	});
	m_node_selector.selected(0);
}
