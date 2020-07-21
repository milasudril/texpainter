//@	{"targets":[{"name":"indexed_sequence.test", "type":"application", "autorun":1}]}

#include "./indexed_sequence.hpp"

#include <string>
#include <cassert>
#include <numbers>
#include <algorithm>

namespace
{
	struct IndexType
	{
		using element_type = size_t;

		element_type m_value;

		auto operator<=>(IndexType const&) const = default;

		auto value() const { return m_value; }
	};
}

namespace Testcases
{
	void texpainterIndexedSequenceAddItems()
	{
		Texpainter::IndexedSequence<double, IndexType, std::string> seq;
		assert(seq.size() == 0);

		seq.append("Pi", static_cast<double>(std::numbers::pi));
		seq.append("Phi", static_cast<double>(std::numbers::phi));
		seq.append("e", static_cast<double>(std::numbers::e));
		assert(seq.size() == 3);

		{
			std::array<double, 3> expected_vals{
			    std::numbers::pi, std::numbers::phi, std::numbers::e};
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

		assert(*seq[IndexType{0}] == std::numbers::pi);
		assert(*seq[IndexType{1}] == std::numbers::phi);
		assert(*seq[IndexType{2}] == std::numbers::e);

		assert(*seq["Pi"] == std::numbers::pi);
		assert(*seq["Phi"] == std::numbers::phi);
		assert(*seq["e"] == std::numbers::e);

		assert(seq["Kalle"] == nullptr);
	}
}

int main()
{
	Testcases::texpainterIndexedSequenceAddItems();
	return 0;
}