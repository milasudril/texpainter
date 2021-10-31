//@	{
//@	 "targets":[{"name":"stack_allocator.test", "type":"application", "autorun":1}]
//@	}

#include "./stack_allocator.hpp"
#include "./default_rng.hpp"

#include <cassert>
#include <algorithm>
#include <random>

namespace Testcases
{
	void texpainterPreallocStackAllocatorFuzzer()
	{
		Texpainter::PreallocStackAllocator<uint64_t> allocator{256};

		std::array<uint64_t*, 256> pointers_in_use{};
		size_t pointers_in_use_size = 0;
		auto& rng = Texpainter::DefaultRng::engine();

		for(size_t k = 0; k < 1024*1024; ++k)
		{
			auto alloc_or_free = std::bernoulli_distribution{static_cast<double>(pointers_in_use_size)/256.0};
			if(!alloc_or_free(rng))
			{
				auto first_free = std::ranges::find(pointers_in_use, nullptr);
				*first_free = create(allocator, rng());
				++pointers_in_use_size;
			}
			else
			{
				auto which = std::uniform_int_distribution{static_cast<size_t>(0), pointers_in_use_size - 1};
				auto index = which(rng);
				assert(pointers_in_use[index] != nullptr);
				destroy(allocator, pointers_in_use[index]);
				pointers_in_use[index] = nullptr;
				--pointers_in_use_size;
				if(index != pointers_in_use_size)
				{ std::shift_left(std::begin(pointers_in_use) + index, std::end(pointers_in_use), 1); }
			}
			std::span valid_range{std::begin(pointers_in_use), pointers_in_use_size};
			std::ranges::sort(valid_range);
			assert(std::begin(std::ranges::unique(valid_range)) == std::end(valid_range));
		}
	}

}

int main()
{
	Testcases::texpainterPreallocStackAllocatorFuzzer();
	return 0;
}