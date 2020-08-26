//@	{"targets":[{"name":"integer_sequence_iterator.test.cpp", "type":"application", "autorun":1}]}

#include "./integer_sequence_iterator.hpp"

#include <algorithm>
#include <cassert>

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
}

int main()
{
	Testcases::texpainterIntegerSequenceIteratorGenSeq();
	Testcases::texpainterIntegerSequenceIteratorGenSeqEnd();
	return 0;
}