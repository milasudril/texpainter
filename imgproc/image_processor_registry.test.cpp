//@	{
//@	 "targets":[{"name":"image_processor_registry.test", "type":"application", "autorun":1}]
//@	}

#include "./image_processor_registry.hpp"

#include <cassert>
#include <ranges>

namespace
{
	struct Compare
	{
		constexpr bool operator()(Texpainter::FilterGraph::ImageProcessorId const& a,
		                          Texpainter::FilterGraph::ImageProcessorId const& b) const
		{
			return std::ranges::lexicographical_compare(a.data(), b.data());
		}

		constexpr bool operator()(char const* a, char const* b) const { return strcmp(a, b) < 0; }
	};
}

namespace Testcases
{
	void processorIdsUnique()
	{
		auto procs = Texpainter::ImageProcessorRegistry::imageProcessorsById();
		auto ids   = std::ranges::transform_view{procs, [](auto const& item) { return item.id; }};
		assert(std::ranges::is_sorted(ids, Compare{}));

		auto i = std::ranges::adjacent_find(ids);
		assert(i == std::end(ids));
	}

	void processorNamesUnique()
	{
		auto procs = Texpainter::ImageProcessorRegistry::imageProcessorsById();
		auto names = std::ranges::transform_view{procs, [](auto const& item) { return item.id; }};
		assert(std::ranges::is_sorted(names, Compare{}));

		auto i = std::ranges::adjacent_find(names);
		assert(i == std::end(names));
	}

	void processorIdsValid()
	{
		auto procs = Texpainter::ImageProcessorRegistry::imageProcessorsById();
		auto ids   = std::ranges::transform_view{procs, [](auto const& item) { return item.id; }};
		assert(std::ranges::find(ids, Texpainter::FilterGraph::InvalidImgProcId) == std::end(ids));
	}
}

int main()
{
	Testcases::processorIdsUnique();
	Testcases::processorNamesUnique();
	Testcases::processorIdsValid();
	return 0;
}
