//@	{"targets":[{"name":"interpolation_table.hpp","type":"include"}]}

#ifndef TEXPAINTER_UTILS_INTERPOLATIONTABLE_HPP
#define TEXPAINTER_UTILS_INTERPOLATIONTABLE_HPP

#include <algorithm>
#include <cmath>

namespace Texpainter
{
	template<class InputType, class OutputType, size_t N>
	requires(N > 1) class InterpolationTable
	{
	public:
		constexpr explicit InterpolationTable(std::array<std::pair<InputType, OutputType>, N> vals)
		{
			std::ranges::sort(vals,
			                  [](auto const& a, auto const& b) { return a.first < b.second; });
			std::ranges::transform(
			    vals, std::begin(m_x), [](auto const& val) { return val.first; });
			std::ranges::transform(
			    vals, std::begin(m_y), [](auto const& val) { return val.second; });
		}

		constexpr OutputType operator()(InputType x) const
		{
			if(x < m_x[0]) [[unlikely]]
				{
					return m_y[0];
				}

			auto const i_max = std::ranges::lower_bound(m_x, x);
			if(i_max > std::end(m_x) - 1) [[unlikely]]
				{
					return m_y[N - 1];
				}

			auto const i_min = i_max - 1;
			auto const n     = i_min - std::begin(m_x);

			auto const t = (x - *i_min) / (*i_max - *i_min);
			return std::lerp(m_y[n], m_y[n + 1], t);
		}

		static constexpr size_t size() { return N; }

	private:
		std::array<InputType, N> m_x;
		std::array<InputType, N> m_y;
	};
}

#endif