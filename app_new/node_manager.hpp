//@	{"targets":[{"name":"node_manager.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_NODEMANAGER_HPP
#define TEXPAINTER_APP_NODEMANAGER_HPP

#include "filtergraph/graph.hpp"

#include <utility>

namespace Texpainter::App
{
	template<class T>
	concept NodeManager = requires(T a)
	{
		a.erase(std::declval<FilterGraph::NodeId>());
		{
			a.copy(std::declval<FilterGraph::NodeId>())
		}
		->std::same_as<FilterGraph::Graph::NodeItem>;
	};

	class ErasedNodeManager
	{
	public:
		template<NodeManager Manager>
		explicit ErasedNodeManager(std::reference_wrapper<Manager> manager)
		    : r_handle{&manager.get()}
		    , m_erase{[](void* handle, FilterGraph::NodeId id) {
			    reinterpret_cast<Manager*>(handle)->erase(id);
		    }}
		    , m_copy{[](void* handle, FilterGraph::NodeId id) {
			    return reinterpret_cast<Manager*>(handle)->copy(id);
		    }}
		{
		}

		auto copy(FilterGraph::NodeId id) { return m_copy(r_handle, id); }
		auto erase(FilterGraph::NodeId id) { return m_erase(r_handle, id); }

	private:
		void* r_handle;
		void (*m_erase)(void*, FilterGraph::NodeId);
		FilterGraph::Graph::NodeItem (*m_copy)(void*, FilterGraph::NodeId);
	};
};

#endif