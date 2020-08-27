//@	{"targets":[{"name":"integer_sequence_iterator.test", "type":"application", "autorun":1}]}

#include "./integer_sequence_iterator.hpp"

#include <algorithm>
#include <cassert>

namespace
{
	class Factorial
	{
	public:
		Factorial(size_t s, size_t c): m_state{s}, m_count{c} {}

		Factorial(): m_state{1}, m_count{0} {}

		Factorial& operator++()
		{
			++m_count;
			m_state = m_state * m_count;
			return *this;
		}

		Factorial operator++(int)
		{
			auto tmp = *this;
			++(*this);
			return tmp;
		}

		auto operator<=>(Factorial const&) const = default;

	private:
		size_t m_state;
		size_t m_count;
	};
}

namespace Testcases
{
	void texpainterIntegerSequenceIteratorGenSeq()
	{
		std::vector<int> some_ints;
		std::copy_n(Texpainter::IntegerSequenceIterator{1}, 3, std::back_inserter(some_ints));

		assert((some_ints == std::vector<int>{1, 2, 3}));
	}

	void texpainterIntegerSequenceIteratorGenSeqEnd()
	{
		std::vector<int> some_ints;
		std::copy(Texpainter::IntegerSequenceIterator{1},
		          Texpainter::IntegerSequenceIterator{4},
		          std::back_inserter(some_ints));

		assert((some_ints == std::vector<int>{1, 2, 3}));
	}

	void texpainterIntegerSequenceIteratorGenFactorial()
	{
		std::vector<Factorial> some_ints;
		std::copy_n(
		    Texpainter::IntegerSequenceIterator{Factorial{}}, 6, std::back_inserter(some_ints));
		assert((some_ints
		        == std::vector<Factorial>{{1, 0}, {1, 1}, {2, 2}, {6, 3}, {24, 4}, {120, 5}}));
	}
}

int main()
{
	Testcases::texpainterIntegerSequenceIteratorGenSeq();
	Testcases::texpainterIntegerSequenceIteratorGenSeqEnd();
	Testcases::texpainterIntegerSequenceIteratorGenFactorial();

	return 0;
}