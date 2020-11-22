//@	{
//@	 "targets":[{"name":"compositor_input.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_MODELNEW_COMPOSITORINPUT_HPP
#define TEXPAINTER_MODELNEW_COMPOSITORINPUT_HPP

#include "./compositor.hpp"
#include "./item_name.hpp"
#include "utils/with_status.hpp"

#include <type_traits>
#include <map>

namespace Texpainter::Model
{
	namespace detail
	{
		template<class T>
		struct ImageProcessor;
	}

	template<class T>
	struct CompositorInput
	{
		T source;
		std::reference_wrapper<
		    FilterGraph::ImageProcessorWrapper<typename detail::ImageProcessor<T>::type>>
		    processor;
	};

	template<class T>
	class CompositorInputManager
	{
	public:
		auto const& get(std::type_identity<T>) const { return m_inputs; }

		auto const get(std::type_identity<T>, ItemName const& item) const
		{
			auto i = m_inputs.find(item);
			return i != std::end(m_inputs) ? &i->second : nullptr;
		}

		auto insert(ItemName const& name,
		            T&& source,
		            Compositor& compositor,
		            std::map<ItemName, Compositor::NodeItem>& nodes)
		{
			auto i = nodes.find(name);
			if(i != std::end(nodes)) [[unlikely]]
				{
					return static_cast<CompositorInput<WithStatus<T>>*>(nullptr);
				}

			using ImgProc = typename detail::ImageProcessor<WithStatus<T>>::type;
			auto item     = compositor.insert(ImgProc{std::string{toString(name)}});
			nodes.insert(i, std::pair{name, item});
			auto& node = dynamic_cast<FilterGraph::ImageProcessorWrapper<ImgProc>&>(
			    item.second.get().processor());
			return &m_inputs
			            .insert(
			                std::pair{name, CompositorInput{WithStatus{std::move(source)}, node}})
			            .first->second;
		}

		bool erase(std::type_identity<T>,
		           ItemName const& item,
		           Compositor& compositor,
		           std::map<ItemName, Compositor::NodeItem>& nodes)
		{
			auto i = nodes.find(item);
			if(i == std::end(nodes)) [[unlikely]]
				{
					return false;
				}

			compositor.erase(i->second.first);
			nodes.erase(i);
			m_inputs.erase(item);
			return true;
		}

		template<InplaceMutator<T> Mutator>
		bool modify(Mutator&& mut, ItemName const& item)
		{
			auto i = m_inputs.find(item);
			if(i == std::end(m_inputs)) [[unlikely]]
				{
					return false;
				}

			return i->second.source.modify(std::forward<Mutator>(mut));
		}

	private:
		std::map<ItemName, CompositorInput<WithStatus<T>>> m_inputs;
	};
}
#endif