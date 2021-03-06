//@	{"targets":[{"name":"sorted_sequence.test", "type":"application", "autorun": 1}]}

#include "./sorted_sequence.hpp"

#include <cassert>
#include <numbers>
#include <algorithm>

namespace Testcases
{
	void texpainterSortedSequenceInsert()
	{
		Texpainter::SortedSequence<std::string, double> seq;
		assert(seq.size() == 0);

		auto res = seq.insert(std::make_pair("pi", std::numbers::pi));
		assert(res.second);
		assert(res.first->first == "pi");

		res = seq.insert(std::make_pair("pi", std::numbers::pi));
		assert(!res.second);
		assert(res.first->first == "pi");
		assert(res.first->second == std::numbers::pi);

		seq.insert(std::make_pair("Phi", std::numbers::phi));
		seq.insert(std::make_pair("e", std::numbers::e));

		{
			assert(seq.size() == 3);
			{
				std::array<std::string, 3> keys{"Phi", "e", "pi"};
				assert(std::ranges::equal(seq.keys(), keys));
			}

			{
				std::array<double, 3> values{std::numbers::phi, std::numbers::e, std::numbers::pi};
				assert(std::ranges::equal(seq.valuesByKey(), values));
			}

			{
				std::array<std::string, 3> keys{"pi", "Phi", "e"};
				assert(std::ranges::equal(seq.keysByIndex(), keys));
			}

			{
				std::array<double, 3> values{std::numbers::pi, std::numbers::phi, std::numbers::e};
				assert(std::ranges::equal(seq.valuesByIndex(), values));
			}

			assert(*seq["Phi"] == std::numbers::phi);
			assert(*seq[1] == std::numbers::phi);
			assert(seq.index("Phi") == 1);
			assert(seq.location("pi") == 2);
		}
	}

	void texpainterSortedSequenceRename()
	{
		Texpainter::SortedSequence<std::string, double> seq;

		seq.insert(std::make_pair("pi", std::numbers::pi));
		seq.insert(std::make_pair("Phi", std::numbers::phi));
		seq.insert(std::make_pair("e", std::numbers::e));

		seq.rename("Phi", "zzz");

		{
			assert(seq.size() == 3);
			{
				std::array<std::string, 3> keys{"e", "pi", "zzz"};
				assert(std::ranges::equal(seq.keys(), keys));
			}

			{
				std::array<double, 3> values{std::numbers::e, std::numbers::pi, std::numbers::phi};
				assert(std::ranges::equal(seq.valuesByKey(), values));
			}

			{
				std::array<std::string, 3> keys{"pi", "zzz", "e"};
				assert(std::ranges::equal(seq.keysByIndex(), keys));
			}

			{
				std::array<double, 3> values{std::numbers::pi, std::numbers::phi, std::numbers::e};
				assert(std::ranges::equal(seq.valuesByIndex(), values));
			}

			assert(seq["Phi"] == nullptr);
			assert(*seq["zzz"] == std::numbers::phi);
			assert(*seq[1] == std::numbers::phi);
			assert(seq.index("Phi") == 0);
			assert(seq.index("zzz") == 1);
		}
	}

	void texpainterSortedSequenceErase()
	{
		Texpainter::SortedSequence<std::string, double> seq;

		seq.insert(std::make_pair("pi", std::numbers::pi));
		seq.insert(std::make_pair("Phi", std::numbers::phi));
		seq.insert(std::make_pair("e", std::numbers::e));

		seq.erase("Phi");

		{
			assert(seq.size() == 2);
			{
				std::array<std::string, 2> keys{"e", "pi"};
				assert(std::ranges::equal(seq.keys(), keys));
			}

			{
				std::array<double, 2> values{std::numbers::e, std::numbers::pi};
				assert(std::ranges::equal(seq.valuesByKey(), values));
			}

			{
				std::array<std::string, 2> keys{"pi", "e"};
				assert(std::ranges::equal(seq.keysByIndex(), keys));
			}

			{
				std::array<double, 2> values{std::numbers::pi, std::numbers::e};
				assert(std::ranges::equal(seq.valuesByIndex(), values));
			}

			assert(seq["Phi"] == nullptr);
			assert(*seq[1] == std::numbers::e);
			assert(*seq[0] == std::numbers::pi);
			assert(seq.index("Phi") == 0);
			assert(seq.index("e") == 1);
			assert(seq.index("pi") == 0);
		}
	}

	void texpainterSortedSequenceMoveForward()
	{
		Texpainter::SortedSequence<std::string, double> seq;

		seq.insert(std::make_pair("pi", std::numbers::pi));
		seq.insert(std::make_pair("Phi", std::numbers::phi));
		seq.insert(std::make_pair("e", std::numbers::e));

		auto res = seq.moveForward(1);
		assert(res == 0);

		{
			assert(seq.size() == 3);
			{
				std::array<std::string, 3> keys{"Phi", "e", "pi"};
				assert(std::ranges::equal(seq.keys(), keys));
			}

			{
				std::array<double, 3> values{std::numbers::phi, std::numbers::e, std::numbers::pi};
				assert(std::ranges::equal(seq.valuesByKey(), values));
			}

			{
				std::array<std::string, 3> keys{"Phi", "pi", "e"};
				assert(std::ranges::equal(seq.keysByIndex(), keys));
			}

			{
				std::array<double, 3> values{std::numbers::phi, std::numbers::pi, std::numbers::e};
				assert(std::ranges::equal(seq.valuesByIndex(), values));
			}

			assert(*seq["Phi"] == std::numbers::phi);
			assert(*seq[0] == std::numbers::phi);
			assert(*seq[1] == std::numbers::pi);
			assert(seq.index("Phi") == 0);
			assert(seq.index("pi") == 1);
		}
	}

	void texpainterSortedSequenceMoveBackward()
	{
		Texpainter::SortedSequence<std::string, double> seq;

		seq.insert(std::make_pair("pi", std::numbers::pi));
		seq.insert(std::make_pair("Phi", std::numbers::phi));
		seq.insert(std::make_pair("e", std::numbers::e));

		auto res = seq.moveBackward(1);
		assert(res == 2);

		{
			assert(seq.size() == 3);
			{
				std::array<std::string, 3> keys{"Phi", "e", "pi"};
				assert(std::ranges::equal(seq.keys(), keys));
			}

			{
				std::array<double, 3> values{std::numbers::phi, std::numbers::e, std::numbers::pi};
				assert(std::ranges::equal(seq.valuesByKey(), values));
			}

			{
				std::array<std::string, 3> keys{"pi", "e", "Phi"};
				assert(std::ranges::equal(seq.keysByIndex(), keys));
			}

			{
				std::array<double, 3> values{std::numbers::pi, std::numbers::e, std::numbers::phi};
				assert(std::ranges::equal(seq.valuesByIndex(), values));
			}

			assert(*seq["Phi"] == std::numbers::phi);
			assert(*seq[1] == std::numbers::e);
			assert(*seq[2] == std::numbers::phi);
			assert(seq.index("Phi") == 2);
			assert(seq.index("e") == 1);
		}
	}

	void texpainterSortedSequenceMoveFront()
	{
		Texpainter::SortedSequence<std::string, double> seq;

		seq.insert(std::make_pair("pi", std::numbers::pi));
		seq.insert(std::make_pair("Phi", std::numbers::phi));
		seq.insert(std::make_pair("e", std::numbers::e));

		seq.moveFront(2);

		{
			assert(seq.size() == 3);
			{
				std::array<std::string, 3> keys{"Phi", "e", "pi"};
				assert(std::ranges::equal(seq.keys(), keys));
			}

			{
				std::array<double, 3> values{std::numbers::phi, std::numbers::e, std::numbers::pi};
				assert(std::ranges::equal(seq.valuesByKey(), values));
			}

			{
				std::array<std::string, 3> keys{"e", "pi", "Phi"};
				assert(std::ranges::equal(seq.keysByIndex(), keys));
			}

			{
				std::array<double, 3> values{std::numbers::e, std::numbers::pi, std::numbers::phi};
				assert(std::ranges::equal(seq.valuesByIndex(), values));
			}

			assert(*seq["e"] == std::numbers::e);
			assert(*seq[0] == std::numbers::e);
			assert(*seq[2] == std::numbers::phi);
			assert(seq.index("Phi") == 2);
			assert(seq.index("e") == 0);
		}
	}

	void texpainterSortedSequenceMoveBack()
	{
		Texpainter::SortedSequence<std::string, double> seq;

		seq.insert(std::make_pair("pi", std::numbers::pi));
		seq.insert(std::make_pair("Phi", std::numbers::phi));
		seq.insert(std::make_pair("e", std::numbers::e));

		seq.moveBack(0);

		{
			assert(seq.size() == 3);
			{
				std::array<std::string, 3> keys{"Phi", "e", "pi"};
				assert(std::ranges::equal(seq.keys(), keys));
			}

			{
				std::array<double, 3> values{std::numbers::phi, std::numbers::e, std::numbers::pi};
				assert(std::ranges::equal(seq.valuesByKey(), values));
			}

			{
				std::array<std::string, 3> keys{"Phi", "e", "pi"};
				assert(std::ranges::equal(seq.keys(), keys));
			}

			{
				std::array<double, 3> values{std::numbers::phi, std::numbers::e, std::numbers::pi};
				assert(std::ranges::equal(seq.valuesByKey(), values));
			}

			assert(*seq["pi"] == std::numbers::pi);
			assert(*seq[0] == std::numbers::phi);
			assert(*seq[2] == std::numbers::pi);
			assert(seq.index("Phi") == 0);
			assert(seq.index("pi") == 2);
		}
	}
}

int main()
{
	Testcases::texpainterSortedSequenceInsert();
	Testcases::texpainterSortedSequenceRename();
	Testcases::texpainterSortedSequenceErase();
	Testcases::texpainterSortedSequenceMoveForward();
	Testcases::texpainterSortedSequenceMoveBackward();
	Testcases::texpainterSortedSequenceMoveFront();
	Testcases::texpainterSortedSequenceMoveBack();
	return 0;
}
