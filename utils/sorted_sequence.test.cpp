//@	{"targets":[{"name":"sorted_sequence.test", "type":"application", "autorun": 1}]}

#include "./sorted_sequence.hpp"

#include <cassert>
#include <numbers>

namespace Testcases
{
	void texpainterSortedSequenceInsert()
	{
		Texpainter::SortedSequence<std::string, double> seq;
		assert(seq.size() == 0);

		auto res = seq.insert(std::make_pair("pi", std::numbers::pi));
		assert(res.second);
		assert(res.first->first == "pi");
		assert(res.first->second == std::numbers::pi);
		assert(seq.size() == 1);
	}

	void texpainterSortedSequenceInsertDuplicate()
	{
		Texpainter::SortedSequence<std::string, double> seq;
		seq.insert(std::make_pair("pi", std::numbers::pi));
		assert(seq.size() == 1);

		auto res = seq.insert(std::make_pair("pi", std::numbers::pi));
		assert(!res.second);
		assert(res.first->first == "pi");
		assert(res.first->second == std::numbers::pi);
		assert(seq.size() == 1);
	}
}

int main()
{
	Testcases::texpainterSortedSequenceInsert();
	Testcases::texpainterSortedSequenceInsertDuplicate();
	return 0;
}

