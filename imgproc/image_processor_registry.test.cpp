//@	{
//@	 "targets":[{"name":"image_processor_registry.test", "type":"application", "autorun":1}]
//@	}

#include "./image_processor_registry.hpp"

#include <cassert>

namespace
{
	struct CompareId
	{
		constexpr bool operator()(Texpainter::FilterGraph::ImageProcessorId const& a,
		                          Texpainter::FilterGraph::ImageProcessorId const& b) const
		{
			return std::ranges::lexicographical_compare(a.data(), b.data());
		}
	};

	struct CompareName
	{
		constexpr bool operator()(char const* a, char const* b) const { return strcmp(a, b) < 0; }
	};
}

namespace Testcases
{
	void processorIdsUnique()
	{
		auto ids = Texpainter::ImageProcessorRegistry::processorIds();
		assert(std::ranges::is_sorted(ids, CompareId{}));

		auto i = std::ranges::adjacent_find(ids);
		assert(i == std::end(ids));
	}

	void processorNamesUnique()
	{
		auto names = Texpainter::ImageProcessorRegistry::processorNames();
		assert(std::ranges::is_sorted(names, CompareName{}));

		auto i = std::ranges::adjacent_find(names);
		assert(i == std::end(names));
	}
}

int main()
{
	Testcases::processorIdsUnique();
	Testcases::processorNamesUnique();

	return 0;
}