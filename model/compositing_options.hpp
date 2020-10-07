//@	{"targets":[{"name":"compositing_options.hpp","type":"include"}]}

#ifndef TEXPAINTER_MODEL_COMPOSITINGOPTIONS_HPP
#define TEXPAINTER_MODEL_COMPOSITINGOPTIONS_HPP

#include "./blend_function.hpp"

#include "utils/maybe_owner.hpp"

#include "filtergraph/graph.hpp"

namespace Texpainter::Model
{
	class CompositingOptions
	{
	public:
		CompositingOptions(): m_filtergraph{FilterGraph::Graph{}}, m_opacity{1.0f} {}

		explicit CompositingOptions(FilterGraph::Graph&& filtergraph,
		                            BlendFunction blend_func,
		                            float opacity)
		    : m_filtergraph{std::move(filtergraph)}
		    , m_blend_func{blend_func}
		    , m_opacity{opacity}
		{
		}

		explicit CompositingOptions(std::reference_wrapper<FilterGraph::Graph const> filtergraph,
		                            BlendFunction blend_func,
		                            float opacity)
		    : m_filtergraph{filtergraph}
		    , m_blend_func{blend_func}
		    , m_opacity{opacity}
		{
		}


		CompositingOptions(CompositingOptions const&) = default;

		CompositingOptions(CompositingOptions&& other) = default;

		CompositingOptions& operator=(CompositingOptions&& other) = default;

		CompositingOptions& operator=(CompositingOptions const& other) = default;

		FilterGraph::Graph const& filterGraph() const { return m_filtergraph.get(); }

		CompositingOptions& filterGraph(FilterGraph::Graph&& filtergraph)
		{
			m_filtergraph = MaybeOwner{std::move(filtergraph)};
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
		MaybeOwner<FilterGraph::Graph> m_filtergraph;
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
