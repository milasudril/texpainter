//@	{
//@	 "targets":[{"name":"compositor_input.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_MODEL_COMPOSITORINPUT_HPP
#define TEXPAINTER_MODEL_COMPOSITORINPUT_HPP

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
		using mapped_type = CompositorInput<WithStatus<T>>;

	public:
		auto const& get(std::type_identity<T>) const { return m_inputs; }

		auto const get(std::type_identity<T>, ItemName const& item) const
		{
			auto i = m_inputs.find(item);
			return i != std::end(m_inputs) ? &i->second : nullptr;
		}

		auto getBefore(std::type_identity<T>, ItemName const& item) const
		{
			using ReturnType = std::pair<ItemName const*, mapped_type const*>;
			auto i           = m_inputs.find(item);
			if(i == std::end(m_inputs)) { return ReturnType{nullptr, nullptr}; }
			if(i == std::begin(m_inputs)) { return ReturnType{&i->first, &i->second}; }
			--i;
			return ReturnType{&i->first, &i->second};
		}

		auto getAfter(std::type_identity<T>, ItemName const& item) const
		{
			using ReturnType = std::pair<ItemName const*, mapped_type const*>;
			auto i           = m_inputs.find(item);
			if(i == std::end(m_inputs)) { return ReturnType{nullptr, nullptr}; }
			auto i_old = i;
			++i;
			if(i == std::end(m_inputs)) { return ReturnType{&i_old->first, &i_old->second}; }
			return ReturnType{&i->first, &i->second};
		}

		auto insert(ItemName const& name,
		            T&& source,
		            Compositor& compositor,
		            std::map<ItemName, Compositor::NodeItem>& nodes)
		{
			auto i = nodes.find(name);
			if(i != std::end(nodes)) [[unlikely]]
				{
					return static_cast<mapped_type*>(nullptr);
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

			if(m_inputs.erase(item) == 0) { return false; }

			compositor.erase(i->second.first);
			nodes.erase(i);
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
		std::map<ItemName, mapped_type> m_inputs;
	};
}
#endif