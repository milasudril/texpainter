//@	{
//@	 "targets":[{"name":"node_output.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_NODEOUTPUT_HPP
#define TEXPAINTER_FILTERGRAPH_NODEOUTPUT_HPP

#include "./node_new.hpp"

#include "utils/inplace_mutator.hpp"

namespace Texpainter::FilterGraph
{
	class NodeOutput
	{
	public:
		explicit NodeOutput(Node& source):m_last_modified{1}, m_last_rendered{0}, m_source{source}
		{}

		Node::result_type const& result() const { return m_result; }

		size_t lastModified() const { return m_last_modified; }

		size_t lastRendered() const { return m_last_rendered; }

		auto inputs() const { return m_source.get().inputs(); }

		template<InplaceMutator<Node> Mutator>
		bool modifyNode(Mutator&& mut)
		{
			if(!mut(m_source.get())) [[unlikely]]
			{ return false; }

			m_last_modified = m_last_rendered + 1;
			return true;
		}

		Node const& source() const
		{
			return m_source.get();
		}

	private:
		size_t m_last_modified;
		size_t m_last_rendered;
		std::reference_wrapper<Node> m_source;
		Node::result_type m_result;
	};

	size_t lastUpdated(NodeOutput const& result)
	{
		return std::max(result.lastModified(), result.lastRendered());
	}

	template<class NodeOutputMap>
	size_t lastUpdated(NodeOutput const& result, NodeOutputMap const& result_map)
	{
		auto last_updated = lastUpdated(result);
		auto inputs = result.source().inputs();
		for(size_t k = 0; k != std::size(inputs); ++k)
		{
			if(!inputs[k].valid()) [[unlikely]]
			{ continue; }

			auto& src = inputs[k].processor();
			auto i = result_map.find(inputs[i].nodeId());
			if(i == std::end(result_map)) [[unlikely]]
			{ continue; }

			last_updated = std::max(lastUpdated(i->second, result_map), last_updated);
		}

		return last_updated;
	}

	template<class NodeOutputMap>
	bool isUpToDate(NodeOutput const& result, NodeOutputMap const& result_map)
	{
		return result.lastRendered() >= lastUpdated(result, result_map);
	}
}

#endif