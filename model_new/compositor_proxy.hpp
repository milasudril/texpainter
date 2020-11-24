//@	{
//@	 "targets":[{"name":"compositor_proxy.hpp", "type":"include"}]
//@	}

#include "./compositor.hpp"

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

		void clearValidationState() { m_compositor.get().clearValidationState(); }

		auto nodesWithId() { return m_compositor.get().nodesWithId(); }

		auto erase(Compositor::NodeId id)
		{
			auto name = m_owner.get().inputNodeName(id);
			if(name == nullptr) [[likely]]
				{
					m_compositor.get().erase(id);
					return true;
				}

			if(!m_owner.get().eraseImage(*name)) [[unlikely]]
				{
					return m_owner.get().erasePalette(*name);
				}

			return true;
		}

		auto insert(std::unique_ptr<Texpainter::FilterGraph::AbstractImageProcessor> proc)
		{
			if(proc->id() == Texpainter::FilterGraph::InvalidImgProcId) [[unlikely]]
				{
					throw std::string{"Failed to insert the selected image processor. Invalid id."};
				}

			return m_compositor.get().insert(std::move(proc));
		}

		std::optional<Compositor::NodeItem> copy(Compositor::NodeId id)
		{
			auto name = m_owner.get().inputNodeName(id);
			if(name == nullptr) [[likely]]
				{
					return insert(m_compositor.get().node(id)->clonedProcessor());
				}
			// FIXME: Return a cookie that can be used to get a new ItemName
			return std::optional<Compositor::NodeItem>{};
		}

	private:
		std::reference_wrapper<NodeManager> m_owner;
		std::reference_wrapper<Compositor> m_compositor;
	};
}
#endif