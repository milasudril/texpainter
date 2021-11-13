//@	{
//@	 "targets":[{"name":"minmax_filter.test","type":"application", "autorun":1}]
//@	}

#include "./minmax_filter.hpp"

#include <cassert>
#include <numeric>
#include <set>

namespace Testcases
{
	void texpainterRollingRankFilterGenXDeltaEllipse6By8()
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

		auto const mask_span = Texpainter::Span2d<int8_t const>{std::data(mask), 9, 7};

		auto x_deltas = Texpainter::RollingRankFilter::genXDelta(mask_span);

		assert(std::size(x_deltas.to_erase) == 6);
		assert(std::size(x_deltas.to_insert) == 6);

		std::multiset<std::pair<uint32_t, uint32_t>> active_set;
		for(uint32_t y = 0; y != mask_span.height() - 1; ++y)
		{
			for(uint32_t x = 0; x != mask_span.width() - 1; ++x)
			{
				if(mask_span(x, y) != 0) { active_set.insert(std::pair{x, y}); }
			}
		}
		assert(std::size(active_set) == 36);

		std::ranges::for_each(x_deltas.to_erase, [&active_set](auto item) {
			auto i = active_set.find(std::pair{item.x, item.y});
			assert(i != std::end(active_set));
			active_set.erase(i);
		});
		assert(std::size(active_set) == 36 - std::size(x_deltas.to_erase));

		std::ranges::for_each(x_deltas.to_insert, [&active_set](auto item) {
			active_set.insert(std::pair{item.x, item.y});
		});

		assert(std::size(active_set) == 36);

		// clang-format off
		constexpr std::array<int8_t, 63> expected_mask_after_move{
		  0, 0, 0, 1, 1, 1, 1, 0, 0,
		  0, 0, 1, 1, 1, 1, 1, 1, 0,
		  0, 1, 1, 1, 1, 1, 1, 1, 1,
		  0, 1, 1, 1, 1, 1, 1, 1, 1,
		  0, 0, 1, 1, 1, 1, 1, 1, 0,
		  0, 0, 0, 1, 1, 1, 1, 0, 0,
		  0, 0, 0, 0, 0, 0, 0, 0, 0
		};
		// clang-format on

		Texpainter::PixelStore::Image<int8_t> mask_after_move{9, 7};
		std::ranges::fill(mask_after_move.pixels(), 0);
		std::ranges::for_each(active_set, [&mask_after_move](auto val) {
			mask_after_move(val.first, val.second) = 1;
		});

		assert(std::ranges::equal(expected_mask_after_move, mask_after_move.pixels()));
	}
}

int main()
{
	Testcases::texpainterRollingRankFilterGenXDeltaEllipse6By8();
	return 0;
}