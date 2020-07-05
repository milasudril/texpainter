//@	{
//@	 "targets":[{"name":"snap.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_APP_SNAP_HPP
#define TEXPAINTER_APP_SNAP_HPP

#include <algorithm>
//#include <array>

namespace Texpainter
{
	template<class T, size_t N>
	class Snap
	{
	public:
		template<class Dummy = int>
		constexpr Snap(std::enable_if_t<N == 0, Dummy> = 0)
		{
		}

		template<class Dummy = int>
		explicit constexpr Snap(T const (&vals)[N], std::enable_if_t<N != 0, Dummy> = 0)
		{
			std::ranges::copy(vals, std::begin(m_vals));
			std::sort(std::begin(m_vals), std::end(m_vals));
		}

		constexpr T nearestValue(T const& val) const
		{
			if constexpr(N == 0) { return val; }
			else
			{
				return m_vals[nearestIndex(val)];
			}
		}

		constexpr size_t nearestIndex(T const& val) const
		{
			static_assert(N != 0);
			auto const i = std::lower_bound(std::begin(m_vals), std::end(m_vals), val);
			if(i == std::begin(m_vals)) { return 0; }
			if(i == std::end(m_vals)) { return std::size(m_vals) - 1; }

			using std::abs;

			auto const d1 = abs(*(i - 1) - val);
			auto const d2 = abs(*i - val);
			return d1 < d2 ? (i - 1) - std::begin(m_vals) : i - std::begin(m_vals);
		}

	private:
		std::array<T, N> m_vals;
	};
}

#endif