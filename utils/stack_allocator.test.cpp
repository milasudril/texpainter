//@	{
//@	 "targets":[{"name":"stack_allocator.test.o", "type":"application"}]
//@	}

#include "./stack_allocator.hpp"
#include <cassert>

namespace Testcases
{
	void texpainterStackAllocatorAllocateBuildAndConsumeFreeList()
	{
		Texpainter::StackAllocator<int> allocator{16};

		assert(allocator.capacity() == 16);
		assert(std::size(allocator.freelist()) == 0);
		assert(std::size(allocator.localContent()) == 0);
	}

}

int main()
{
	Testcases::texpainterStackAllocatorAllocateBuildAndConsumeFreeList();
	return 0;
}