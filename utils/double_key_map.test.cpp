//@	{"targets":[{"name":"double_key_map.test", "type":"application", "autorun":1}]}

#include "./double_key_map.hpp"

#include <string>
#include <cassert>
#include <numbers>
#include <algorithm>

namespace Testcases
{
	void texpainterDoubleMapInsert()
	{
		Texpainter::DoubleKeyMap<double, int, std::string> map;
		assert(map.size() == 0);

		map.insert(static_cast<double>(std::numbers::pi), 0,  "pi");
		map.insert(static_cast<double>(std::numbers::phi), 1,  "Phi");
		map.insert(static_cast<double>(std::numbers::e), 2, "e");

		assert(map.size() == 3);

		assert(*map[0] == std::numbers::pi);
		assert(*map[1] == std::numbers::phi);
		assert(*map[2] == std::numbers::e);

		assert(*map["pi"] == std::numbers::pi);
		assert(*map["Phi"] == std::numbers::phi);
		assert(*map["e"] == std::numbers::e);

#if 0
		{
			std::array<double, 3> expected_vals{std::numbers::pi, std::numbers::phi, std::numbers::e};
			assert(std::ranges::equal(seq.valuesInSequence(), expected_vals));
		}

		{
			std::array<std::string, 3> expected_names{"Pi", "Phi", "e"};
			assert(std::ranges::equal(seq.namesInSequence(), expected_names));
		}

		{
			std::array<std::string, 3> expected_names{"Phi", "Pi", "e"};
			assert(std::ranges::equal(seq.namesInOrder(), expected_names));
		}

		{
			std::array<IndexType, 3> expected_indices{IndexType{1}, IndexType{0}, IndexType{2}};
			assert(std::ranges::equal(seq.indicesInOrder(), expected_indices));
		}



		assert(*seq["Pi"] == std::numbers::pi);
		assert(*seq["Phi"] == std::numbers::phi);
		assert(*seq["e"] == std::numbers::e);

		assert(seq["Kalle"] == nullptr);
#endif
	}
}

int main()
{
	Testcases::texpainterDoubleMapInsert();
	return 0;
}