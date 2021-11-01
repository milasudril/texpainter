//@	{
//@	 "targets":[{"name":"preallocated_multiset.test", "type":"application", "autorun":1}]
//@	}

#include "./preallocated_multiset.hpp"
#include "./default_rng.hpp"

namespace Testcases
{
	void texpainterPreallocatedMultiset()
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
		vals.erase_one(static_cast<int>(freq_max - std::begin(hist)));
		assert(std::size(vals) == 15);
	}
}

int main()
{
	Testcases::texpainterPreallocatedMultiset();
	return 0;
}