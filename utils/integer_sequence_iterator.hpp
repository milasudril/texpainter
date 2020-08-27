//@	{"targets":[{"name":"integer_sequence_iterator.hpp", "type":"include"}]}

#ifndef TEXPAINTER_UTILS_PAIRITERATOR_HPP
#define TEXPAINTER_UTILS_PAIRITERATOR_HPP

#include <iterator>
#include <type_traits>
#include <utility>

namespace Texpainter
{
	template<class Integer>
	class IntegerSequenceIterator
	{
	public:
		using difference_type   = intptr_t;
		using value_type        = Integer;
		using iterator_category = std::input_iterator_tag;

		IntegerSequenceIterator() = default;

		IntegerSequenceIterator(IntegerSequenceIterator const&) = default;

		explicit IntegerSequenceIterator(Integer init_val): m_i{init_val} {}

		auto operator<=>(IntegerSequenceIterator const& other) const = default;

		auto operator*() const { return m_i; }


		IntegerSequenceIterator& operator++()
		{
			++m_i;
			return *this;
		}

		IntegerSequenceIterator operator++(int)
		{
			auto tmp = *this;
			++(*this);
			return tmp;
		}

		auto rawIterator() const { return m_i; }

	private:
		Integer m_i;
	};
}

#endif