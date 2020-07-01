//@	{"targets":[{"name":"sequence.hpp", "type":"include"}]}

#ifndef TEXPAINTER_SEQUENCE_HPP
#define TEXPAINTER_SEQUENCE_HPP

#include <vector>
#include <cassert>

namespace Texpainter
{
	template<class T, class IndexType, class BackingStore = std::vector<T>>
	class Sequence: private BackingStore
	{
		using Base = BackingStore;

	public:
		using Base::begin;
		using Base::end;
		using index_type = IndexType;
		using size_type = typename IndexType::element_type;
		using value_type = T;

		auto size() const
		{
			return static_cast<size_type>(Base::size());
		}

		auto& append(T&& layer)
		{
			Base::push_back(std::move(layer));
			return *this;
		}

		auto& remove(index_type index)
		{
			assert(index.value() < size());
			Base::erase(begin() + index.value());
		}

		static auto firstIndex()
		{ return index_type{0}; }

		auto lastIndex() const
		{
			return index_type{size() - 1};
		}

		auto& moveForward(index_type index)
		{
			assert(index.value() < size());
			if(index != firstIndex())
			{
				auto i = begin() + index.value();
				std::swap(*(i - 1), *i);
			}
			return *this;
		}

		auto& moveBackward(index_type index)
		{
			assert(index.value() < size());
			if(index != lastIndex())
			{
				auto i = begin() + index.value();
				std::swap(*(i + 1), *i);
			}
			return *this;
		}

		auto& operator[](index_type index)
		{
			return *(begin() + index.value());
		}

		auto const& operator[](index_type index) const
		{
			return *(begin() + index.value());
		}
	};
}

#endif