//@	{
//@	 "targets":[{"name":"compositor_proxy.hpp", "type":"include"}]
//@	}

#include "./compositor.hpp"
#include "./item_name.hpp"

#ifndef TEXPAINTER_MODEL_COMPOSITORPROXY_HPP
#define TEXPAINTER_MODEL_COMPOSITORPROXY_HPP

namespace Texpainter::Model
{
	template<class NodeManager>
	class CompositorProxy
	{
	public:
		explicit CompositorProxy(NodeManager& owner, Compositor& comp)
		    : m_owner{owner}
		    , m_compositor{comp}
		{
		}

		CompositorProxy& connect(Compositor::NodeId a,
		                         Compositor::InputPortIndex sink,
		                         Compositor::NodeId b,
		                         Compositor::OutputPortIndex src)
		{
			m_compositor.get().connect(a, sink, b, src);
			return *this;
		}

		auto addTopoOutput() { return m_compositor.get().addTopoOutput(); }

		bool checkedConnect(Compositor::NodeId a,
		                    Compositor::InputPortIndex sink,
		                    Compositor::NodeId b,
		                    Compositor::OutputPortIndex src)
		{
			return m_compositor.get().checkedConnect(a, sink, b, src);
		}

		void clearValidationState() { m_compositor.get().clearValidationState(); }

		auto nodesWithId() { return m_compositor.get().nodesWithId(); }

		auto erase(Compositor::NodeId id) { return m_owner.get().erase(id); }

		auto insert(std::unique_ptr<Texpainter::FilterGraph::AbstractImageProcessor> proc)
		{
			if(proc->id() == Texpainter::FilterGraph::InvalidImgProcId) [[unlikely]]
				{
					throw std::string{"Failed to insert the selected image processor. Invalid id."};
				}

			return m_compositor.get().insert(std::move(proc));
		}

		template<auto CtrlId, class EventHandler>
		void copy(EventHandler& eh, Compositor::NodeId id)
		{
			auto name = m_owner.get().inputNodeName(id);
			if(name == nullptr) [[likely]]
				{
					eh.template onCopyCompleted<CtrlId>(
					    id, insert(m_compositor.get().node(id)->clonedProcessor()));
					return;
				}
			eh.template requestItemName<CtrlId>(id, *this);
		}

		template<auto CtrlId, class EventHandler>
		void insertNodeWithName(EventHandler& eh, Compositor::NodeId id, ItemName const& name_new)
		{
			if(auto node_item = m_owner.get().copy(id, name_new); node_item != nullptr) [[likely]]
				{
					eh.template onCopyCompleted<CtrlId>(id, *node_item);
					return;
				}

			throw std::string{"An item with the given name already exist."};
		}

	private:
		std::reference_wrapper<NodeManager> m_owner;
		std::reference_wrapper<Compositor> m_compositor;
	};
}

#endif