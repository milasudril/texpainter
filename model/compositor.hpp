//@	{
//@	 "targets":[{"name":"compositor.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_MODEL_COMPOSITOR_HPP
#define TEXPAINTER_MODEL_COMPOSITOR_HPP

#include "filtergraph/graph.hpp"
#include "pixel_store/image.hpp"
#include "utils/iter_pair.hpp"
#include "utils/pair_iterator.hpp"
#include "sched/signaling_counter.hpp"

#include <algorithm>

namespace Texpainter::Model
{
	class Compositor
	{
	public:
		static constexpr auto InputNodeId  = FilterGraph::Graph::InputNodeId;
		static constexpr auto OutputNodeId = FilterGraph::Graph::OutputNodeId;
		using NodeItem                     = FilterGraph::Graph::NodeItem;

		PixelStore::Image process(FilterGraph::Input const& input, bool force_update = true) const
		{
			return m_graph.process(input, force_update);
		}

		bool valid() const { return m_graph.valid(); }

		void clearValidationState() { m_graph.clearValidationState(); }

		NodeItem insert(std::unique_ptr<FilterGraph::AbstractImageProcessor> proc)
		{
			return m_graph.insert(std::move(proc));
		}

		template<FilterGraph::ImageProcessor ImgProc>
		NodeItem insert(ImgProc&& proc)
		{
			return m_graph.insert(std::forward<ImgProc>(proc));
		}

		auto node(FilterGraph::NodeId id) const { return m_graph.node(id); }

		Compositor& erase(FilterGraph::NodeId id)
		{
			m_graph.erase(id);
			return *this;
		}

		auto nodesWithId() const { return m_graph.nodesWithId(); }

		auto nodesWithId() { return m_graph.nodesWithId(); }

	private:
		FilterGraph::Graph m_graph;
	};
}

#endif