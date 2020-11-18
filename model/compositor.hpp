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
	class Compositor: public FilterGraph::Graph
	{
	public:
	};
}

#endif