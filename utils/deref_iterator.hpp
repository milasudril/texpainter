//@	{"targets":[{"name":"deref_iterator.hpp", "type":"include"}]}

#ifndef TEXPAINTER_DEREFITERATOR_HPP
#define TEXPAINTER_DEREFITERATOR_HPP

#include <iterator>
#include <type_traits>

namespace Texpainter
{
	template<class Iter>
	class DerefIterator
	{
	public:
		using difference_type   = intptr_t;
		using value_type        = std::remove_cvref_t<decltype(**std::declval<Iter>())>;
		using iterator_category = std::bidirectional_iterator_tag;

		DerefIterator() = default;

		DerefIterator(DerefIterator const&) = default;

		explicit DerefIterator(Iter i): m_i{i} {}

		auto operator<=>(DerefIterator const& other) const = default;

		auto& operator*() const { return *(*m_i); }

		auto operator->() const { return &(*(*this)); };


		DerefIterator& operator++()
		{
			++m_i;
			return *this;
		}

		DerefIterator& operator--()
		{
			--m_i;
			return *this;
		}

		DerefIterator operator++(int)
		{
			auto tmp = *this;
			++(*this);
			return tmp;
		}

		DerefIterator operator--(int)
		{
			auto tmp = *this;
			--(*this);
			return tmp;
		}

		auto rawIterator() const { return m_i; }

	private:
		Iter m_i;
	};
}

#endif