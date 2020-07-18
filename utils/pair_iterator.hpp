//@	{"targets":[{"name":"pair_iterator.hpp", "type":"include"}]}

#ifndef TEXPAINTER_PAIRITERATOR_HPP
#define TEXPAINTER_PAIRITERATOR_HPP

#include <iterator>
#include <type_traits>
#include <utility>

namespace Texpainter
{
	template<size_t item, class Iter>
	class PairIterator
	{
	public:
		using difference_type = intptr_t;
		using value_type = std::remove_cvref_t<decltype(std::get<item>(*std::declval<Iter>()))>;
		using iterator_category = std::bidirectional_iterator_tag;

		PairIterator() = default;

		PairIterator(PairIterator const&) = default;

		explicit PairIterator(Iter i): m_i{i}
		{
		}

		auto operator<=>(PairIterator const& other) const = default;

		auto& operator*() const
		{
			return std::get<item>(*m_i);
		}

		auto operator-> () const
		{
			return &(*(*this));
		};


		PairIterator& operator++()
		{
			++m_i;
			return *this;
		}

		PairIterator& operator--()
		{
			--m_i;
			return *this;
		}

		PairIterator operator++(int)
		{
			auto tmp = *this;
			++(*this);
			return tmp;
		}

		PairIterator operator--(int)
		{
			auto tmp = *this;
			--(*this);
			return tmp;
		}

		auto rawIterator() const
		{
			return m_i;
		}

	private:
		Iter m_i;
	};

	template<class Iter>
	using PairFirstIterator = PairIterator<0, Iter>;

	template<class Iter>
	using PairSecondIterator = PairIterator<1, Iter>;
}

#endif