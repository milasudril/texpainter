//@	{"targets":[{"name":"iter_pair.test", "type":"object"}]}

#include "./iter_pair.hpp"
#include "./deref_iterator.hpp"

#include <ranges>
#include <cassert>
#include <algorithm>

void do_stuff(int);

void test(int** begin, int** end)
{
	Texpainter::IterPair range{Texpainter::DerefIterator{begin}, Texpainter::DerefIterator{end}};

	std::ranges::for_each(range, [](int val) { do_stuff(val); });
}