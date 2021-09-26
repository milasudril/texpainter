//@	{
//@	 "targets":[{"name":"node_output.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_NODEOUTPUT_HPP
#define TEXPAINTER_FILTERGRAPH_NODEOUTPUT_HPP

#include "./node_new.hpp"

#include "utils/inplace_mutator.hpp"

#include <map>

namespace Texpainter::FilterGraph
{
	class NodeOutput
	{
	public:
		explicit NodeOutput(Node& source):m_last_modified{1}, m_last_rendered{0}, m_source{source}
		{}

		Node::result_type const& result() const { return m_result; }

		bool isUpToDate() const { return m_last_modified <= m_last_rendered; }

		auto inputs() const { return m_source.get().inputs(); }

		template<InplaceMutator<Node> Mutator>
		bool modifyNode(Mutator&& mut)
		{
			if(!mut(m_source.get())) [[unlikely]]
			{ return false; }

			m_last_modified = m_last_rendered + 1;
			return true;
		}

	private:
		size_t m_last_modified;
		size_t m_last_rendered;
		std::reference_wrapper<Node> m_source;
		Node::result_type m_result;
	};
}

#endif