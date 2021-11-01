//@	{
//@	 "targets":[{"name":"preallocated_multiset.test", "type":"application", "autorun":1}]
//@	}

#include "./preallocated_multiset.hpp"
#include "./default_rng.hpp"

#include <cassert>

namespace Testcases
{
	void texpainterPreallocatedMultisetCreateInsertAndErase()
	{
		Texpainter::PreallocatedMultiset<int> vals{16};
		auto& rng = Texpainter::DefaultRng::engine();

		std::array<int, 16> inserted_vals{};

		std::uniform_int_distribution u{0, 8};
		for(size_t k = 0; k != 16; ++k)
		{
			inserted_vals[k] = u(rng);
			vals.insert(inserted_vals[k]);
		}
		std::ranges::sort(inserted_vals);
		assert(std::ranges::equal(vals, inserted_vals));

		std::array<int, 8> hist{};
		std::ranges::for_each(vals, [&hist](auto val) { hist[val]++; });
		auto freq_max = std::ranges::max_element(hist);
		assert(*freq_max > 1);
		auto const erase_val = static_cast<int>(freq_max - std::begin(hist));
		vals.erase_one(erase_val);
		assert(std::size(vals) == 15);

		assert(*freq_max > 2);
		auto const n = vals.erase(erase_val);
		assert(static_cast<int>(n) == *freq_max - 1);
		assert(std::size(vals) == static_cast<size_t>(16 - *freq_max));

		vals.insert(35);
	}

	void texpainterPreallocatedMultisetCreateFillAndMove()
	{
		Texpainter::PreallocatedMultiset<int> vals{16};
		std::generate_n(std::inserter(vals, std::end(vals)), 8, [k = 0]() mutable {
			++k;
			return k;
		});
		assert(std::size(vals) == 8);

		auto other = std::move(vals);
		assert(std::size(other) == 8);
		assert(std::size(vals) == 0);

		other.insert(9);

		std::ranges::for_each(other, [k = 0](auto item) mutable {
			++k;
			assert(item == k);
		});
	}
}

int main()
{
	Testcases::texpainterPreallocatedMultisetCreateInsertAndErase();
	Testcases::texpainterPreallocatedMultisetCreateFillAndMove();
	return 0;
}