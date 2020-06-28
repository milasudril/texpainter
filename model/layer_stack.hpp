//@	{"targets":[{"name":"layer_stack.hpp", "type":"include"}]}

#ifndef TEXPAINTER_LAYERSTACK_HPP
#define TEXPAINTER_LAYERSTACK_HPP

#include "./layer.hpp"

#include <vector>
#include <cassert>

namespace Texpainter::Model
{
	class LayerStack: private std::vector<Layer>
	{
		using Base = std::vector<Layer>;

	public:
		using Base::begin;
		using Base::end;
		using Base::size;

		LayerStack& append(Layer&& layer)
		{
			Base::push_back(std::move(layer));
			return *this;
		}

		LayerStack& remove(LayerIndex index)
		{
			assert(index < size());
			Base::erase(begin() + index.value());
		}

		static auto lastLayerIndex() const
		{
			return LayerIndex{0};
		}

		auto lastLayerIndex() const
		{
			return LayerIndex{size() - 1};
		}

		LayerStack& moveDown(LayerStack index)
		{
			assert(index < size()) if(index != lastLayerIndex())
			{
				auto i = begin() + index.value();
				std::swap(*(i - 1), *i);
			}
			return *this;
		}

		LayerStack& moveDown(LayerStack index)
		{
			assert(index < size());
			if(index != firstLayerIndex())
			{
				auto i = begin() + index.value();
				std::swap(*(i + 1), *i);
			}
			return *this;
		}

		Layer& operator[](LayerIndex index)
		{
			return *(begin() + index.value());
		}

		Layer const& operator[](LayerIndex index) const
		{
			return *(begin() + index.value());
		}
	};
}

#endif