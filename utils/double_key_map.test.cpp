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

		map.insert(static_cast<double>(std::numbers::pi), 0, "pi");
		map.insert(static_cast<double>(std::numbers::phi), 1, "Phi");
		map.insert(static_cast<double>(std::numbers::e), 2, "e");

		assert(map.size() == 3);

		assert(*map[0] == std::numbers::pi);
		assert(*map[1] == std::numbers::phi);
		assert(*map[2] == std::numbers::e);

		assert(*map["pi"] == std::numbers::pi);
		assert(*map["Phi"] == std::numbers::phi);
		assert(*map["e"] == std::numbers::e);

		assert(map["Kalle"] == nullptr);
		assert(map[1243] == nullptr);


		{
			std::array<double, 3> expected_vals{std::numbers::pi, std::numbers::phi, std::numbers::e};
			assert(std::ranges::equal(map.valuesByFirstKey(), expected_vals));
		}

		{
			std::array<double, 3> expected_vals{std::numbers::phi, std::numbers::e, std::numbers::pi};
			assert(std::ranges::equal(map.valuesBySecondKey(), expected_vals));
		}

		{
			std::array<int, 3> expected_keys{0, 1, 2};
			assert(std::ranges::equal(map.firstKey(), expected_keys));
		}

		{
			std::array<std::string, 3> expected_keys{"Phi", "e", "pi"};
			assert(std::ranges::equal(map.secondKey(), expected_keys));
		}
	}

	void texpainterDoubleMapRemoveByFirst()
	{
		Texpainter::DoubleKeyMap<double, int, std::string> map;

		map.insert(static_cast<double>(std::numbers::pi), 0, "pi");
		map.insert(static_cast<double>(std::numbers::phi), 1, "Phi");
		map.insert(static_cast<double>(std::numbers::e), 2, "e");

		map.remove(1);

		assert(map.size() == 2);

		assert(*map[0] == std::numbers::pi);
		assert(map[1] == nullptr);
		assert(*map[2] == std::numbers::e);

		assert(*map["pi"] == std::numbers::pi);
		assert(map[1] == nullptr);
		assert(*map["e"] == std::numbers::e);
	}

	void texpainterDoubleMapRemoveBySecond()
	{
		Texpainter::DoubleKeyMap<double, int, std::string> map;

		map.insert(static_cast<double>(std::numbers::pi), 0, "pi");
		map.insert(static_cast<double>(std::numbers::phi), 1, "Phi");
		map.insert(static_cast<double>(std::numbers::e), 2, "e");

		map.remove("Phi");

		assert(map.size() == 2);

		assert(*map[0] == std::numbers::pi);
		assert(map[1] == nullptr);
		assert(*map[2] == std::numbers::e);

		assert(*map["pi"] == std::numbers::pi);
		assert(map[1] == nullptr);
		assert(*map["e"] == std::numbers::e);
	}

	void texpainterDoubleMapInsertFirstKeyExists()
	{
		Texpainter::DoubleKeyMap<double, int, std::string> map;

		map.insert(static_cast<double>(std::numbers::pi), 0, "pi");
		map.insert(static_cast<double>(std::numbers::phi), 1, "Phi");
		map.insert(static_cast<double>(std::numbers::e), 2, "e");

		assert((map.insert(214.3, 2, "Foo")
		        == Texpainter::DoubleKeyMap<double, int, std::string>::InsertResult::FirstKeyExists));
	}

	void texpainterDoubleMapInsertSecondKeyExists()
	{
		Texpainter::DoubleKeyMap<double, int, std::string> map;

		map.insert(static_cast<double>(std::numbers::pi), 0, "pi");
		map.insert(static_cast<double>(std::numbers::phi), 1, "Phi");
		map.insert(static_cast<double>(std::numbers::e), 2, "e");

		assert((map.insert(214.3, 4, "e")
		        == Texpainter::DoubleKeyMap<double, int, std::string>::InsertResult::SecondKeyExists));
	}

	void texpainterDoubleMapInsertBothKeysExists()
	{
		Texpainter::DoubleKeyMap<double, int, std::string> map;

		map.insert(static_cast<double>(std::numbers::pi), 0, "pi");
		map.insert(static_cast<double>(std::numbers::phi), 1, "Phi");
		map.insert(static_cast<double>(std::numbers::e), 2, "e");

		assert((map.insert(214.3, 2, "pi")
		        == Texpainter::DoubleKeyMap<double, int, std::string>::InsertResult::BothKeysExist));
	}
}

int main()
{
	Testcases::texpainterDoubleMapInsert();
	Testcases::texpainterDoubleMapRemoveByFirst();
	Testcases::texpainterDoubleMapRemoveBySecond();
	Testcases::texpainterDoubleMapInsertFirstKeyExists();
	Testcases::texpainterDoubleMapInsertSecondKeyExists();
	Testcases::texpainterDoubleMapInsertBothKeysExists();
	return 0;
}