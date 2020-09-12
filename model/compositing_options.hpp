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
		CompositingOptions(): m_filtergraph{std::make_unique<FilterGraph::Graph>()}, m_opacity{1.0f}
		{
		}

		explicit CompositingOptions(FilterGraph::Graph const& filtergraph,
		                            BlendFunction blend_func,
		                            float opacity)
		    : m_filtergraph{std::make_unique<FilterGraph::Graph>(filtergraph)}
		    , m_blend_func{blend_func}
		    , m_opacity{opacity}
		{
		}

		CompositingOptions(CompositingOptions const& other)
		    : CompositingOptions{other.filterGraph(), other.blendFunction(), other.opacity()}
		{
		}

		CompositingOptions(CompositingOptions&& other) = default;

		CompositingOptions& operator=(CompositingOptions&& other) = default;

		CompositingOptions& operator=(CompositingOptions const& other)
		{
			auto temp = other;
			*this     = std::move(other);
			return *this;
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

		float opacity() const { return m_opacity; }

		CompositingOptions& opacity(float val)
		{
			m_opacity = val;
			return *this;
		}

	private:
		std::unique_ptr<FilterGraph::Graph> m_filtergraph;
		BlendFunction m_blend_func;
		float m_opacity;
	};


	inline PixelStore::Pixel blend(CompositingOptions const& composit_options,
	                               PixelStore::Pixel dest,
	                               PixelStore::Pixel src)
	{
		return composit_options.blendFunction()(dest, src, composit_options.opacity());
	}
}

#endif
