//@	{
//@	 "targets":[{"name":"stack_allocator.test.o", "type":"application"}]
//@	}

#include "./stack_allocator.hpp"

#include <cassert>
#include <algorithm>

namespace Testcases
{
	void texpainterStackAllocatorAllocateFreeAndConsumeFreeList()
	{
		Texpainter::StackAllocator<int> allocator{16};

		assert(allocator.capacity() == 16);
		assert(std::size(allocator.freelist()) == 0);
		assert(std::size(allocator.localContent()) == 0);

		std::array<int*, 16> vals{};

		for(size_t k = 0; k != allocator.capacity(); ++k)
		{
			vals[k] = new(allocator.allocate(1))int{static_cast<int>(k)};
		}

		assert(std::size(allocator.localContent()) == allocator.capacity());
		std::ranges::sort(vals);
		assert(std::begin(std::ranges::unique(vals)) == std::end(vals));

		std::ranges::for_each(allocator.localContent(), [k = 0](int item) mutable {
			assert(item == k);
			++k;
		});

		for(size_t k = 0; k != allocator.capacity(); ++k)
		{
			allocator.deallocate(vals[k], 1);
		}

		assert(std::size(allocator.freelist()) == allocator.capacity());
		for(size_t k = 0; k != allocator.capacity(); ++k)
		{
			assert(allocator.freelist()[k] == k);
		}

		for(size_t k = 0; k != allocator.capacity(); ++k)
		{
			vals[k] = new(allocator.allocate(1))int{static_cast<int>(k)};
		}

		assert(std::size(allocator.freelist()) == 0);


	}

}

int main()
{
	Testcases::texpainterStackAllocatorAllocateFreeAndConsumeFreeList();
	return 0;
}