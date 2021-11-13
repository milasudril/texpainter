//@	{
//@	 "targets":[{"name":"minmax_filter.test","type":"application", "autorun":1}]
//@	}

#include "./minmax_filter.hpp"

#include <cassert>
#include <numeric>

namespace Testcases
{
	void texpainterRollingRankFilterGenLineDeltaEllipse6By8()
	{
		// clang-format off
		constexpr std::array<int8_t, 63> mask{
		 0, 0, 1, 1, 1, 1, 0, 0, 0,
		 0, 1, 1, 1, 1, 1, 1, 0, 0,
		 1, 1, 1, 1, 1, 1, 1, 1, 0,
		 1, 1, 1, 1, 1, 1, 1, 1, 0,
		 0, 1, 1, 1, 1, 1, 1, 0, 0,
		 0, 0, 1, 1, 1, 1, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0, 0};
		// clang-format on

		auto line_deltas = Texpainter::RollingRankFilter::genLineDelta(
		    Texpainter::Span2d<int8_t const>{std::data(mask), 9, 7});

		assert(std::size(line_deltas) == 12);
		assert((std::ranges::max_element(line_deltas, [](auto a, auto b) { return a.x < b.x; })->x
		        == 8));
		assert((std::ranges::min_element(line_deltas, [](auto a, auto b) { return a.x < b.x; })->x
		        == 0));
		assert((std::ranges::max_element(line_deltas, [](auto a, auto b) { return a.y < b.y; })->y
		        == 5));
		assert((std::ranges::min_element(line_deltas, [](auto a, auto b) { return a.y < b.y; })->y
		        == 0));

		Texpainter::PixelStore::Image<int8_t> delta_mask{9, 6};
		std::ranges::fill(delta_mask.pixels(), 0);
		std::ranges::for_each(line_deltas, [&delta_mask](auto val) {
			delta_mask(val.x, val.y) = static_cast<int8_t>(val.sign);
		});

		auto const sum = std::accumulate(std::begin(line_deltas),
		                                 std::end(line_deltas),
		                                 0,
		                                 [](int32_t a, auto b) { return a + b.sign; });
		assert(sum == 0);

		// clang-format off
		constexpr std::array<int8_t, 54> mask_res{
		  0,  0, -1, 0, 0, 0, 1, 0, 0,
		  0, -1,  0, 0, 0, 0, 0, 1, 0,
		 -1,  0,  0, 0, 0, 0, 0, 0, 1,
		 -1,  0,  0, 0, 0, 0, 0, 0, 1,
		  0, -1,  0, 0, 0, 0, 0, 1, 0,
		  0,  0, -1, 0, 0, 0, 1, 0, 0};
		// clang-format on

		assert(std::ranges::equal(mask_res, delta_mask.pixels()));
	}

	void texpainterRollingRankFilterGenLineDelta1By1()
	{
		// clang-format off
		constexpr std::array<int8_t, 4> mask{
		 1, 0,
		 0, 0};
		// clang-format on

		auto line_deltas = Texpainter::RollingRankFilter::genLineDelta(
		    Texpainter::Span2d<int8_t const>{std::data(mask), 2, 2});

		assert(std::size(line_deltas) == 2);
	}
}

int main()
{
	Testcases::texpainterRollingRankFilterGenLineDeltaEllipse6By8();
	Testcases::texpainterRollingRankFilterGenLineDelta1By1();
	return 0;
}