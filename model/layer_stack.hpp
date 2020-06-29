//@	{"targets":[{"name":"layer_stack.hpp", "type":"include"}]}

#ifndef TEXPAINTER_LAYERSTACK_HPP
#define TEXPAINTER_LAYERSTACK_HPP

#include "./layer.hpp"
#include "./layer_index.hpp"

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

		LayerIndex::element_type size() const
		{
			return static_cast<LayerIndex::element_type>(Base::size());
		}

		LayerStack& append(Layer&& layer)
		{
			Base::push_back(std::move(layer));
			return *this;
		}

		LayerStack& remove(LayerIndex index)
		{
			assert(index.value() < size());
			Base::erase(begin() + index.value());
		}

		static auto lastLayerIndex()
		{
			return LayerIndex{0};
		}

		auto firstLayerIndex() const
		{
			return LayerIndex{size() - 1};
		}

		LayerStack& moveDown(LayerIndex index)
		{
			assert(index.value() < size());
			if(index != lastLayerIndex())
			{
				auto i = begin() + index.value();
				std::swap(*(i - 1), *i);
			}
			return *this;
		}

		LayerStack& moveUp(LayerIndex index)
		{
			assert(index.value() < size());
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

	void render(LayerStack const& layers, Span2d<Pixel> ret)
	{
		std::ranges::for_each(layers, [ret](auto const& layer) {
			render(layer, ret);
		});
	}
}

#endif