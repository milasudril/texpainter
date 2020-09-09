//@	{"targets":[{"name":"compositing_options.hpp","type":"include"}]}

#ifndef TEXPAINTER_MODEL_COMPOSITINGOPTIONS_HPP
#define TEXPAINTER_MODEL_COMPOSITINGOPTIONS_HPP

#include "./blend_function.hpp"

#include "filtergraph/graph.hpp"

namespace Texpainter::Model
{
	class CompositingOptions
	{
	public:
		explicit CompositingOptions(FilterGraph::Graph const& filtergraph, BlendFunction blend_func)
		    : m_filtergraph{std::make_unique<FilterGraph::Graph>(filtergraph)}
		    , m_blend_func{blend_func}
		{
		}

		FilterGraph::Graph const& filterGraph() const { return *m_filtergraph; }

		CompositingOptions& filterGraph(FilterGraph::Graph const& filtergraph)
		{
			m_filtergraph = std::make_unique<FilterGraph::Graph>(filtergraph);
			return *this;
		}

		BlendFunction blendFunction() const { return m_blend_func; }

		CompositingOptions& blendFunction(BlendFunction blend_func)
		{
			m_blend_func = blend_func;
			return *this;
		}

	private:
		std::unique_ptr<FilterGraph::Graph> m_filtergraph;
		BlendFunction m_blend_func;
	};
}

#endif
