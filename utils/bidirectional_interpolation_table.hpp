//@	{"targets":[{"name":"bidirectional_interpolation_table.hpp","type":"include"}]}

#ifndef TEXPAINTER_UTILS_BIDIRECTIONALINTERPOLATIONTABLE_HPP
#define TEXPAINTER_UTILS_BIDIRECTIONALINTERPOLATIONTABLE_HPP

#include "./interpolation_table.hpp"

#include <algorithm>
#include <cmath>

namespace Texpainter
{
	namespace detail
	{
		template<class A, class B, size_t N>
		constexpr std::array<std::pair<B, A>, N> swap_pairs(
		    std::array<std::pair<A, B>, N> const& input)
		{
			std::array<std::pair<B, A>, N> ret;
			std::ranges::transform(input, std::begin(ret), [](auto const& val) {
				return std::pair{val.second, val.first};
			});
			return ret;
		}
	}

	template<class InputType, class OutputType, size_t N>
	requires(N > 1) class BidirectionalInterpolationTable
	{
	public:
		constexpr explicit BidirectionalInterpolationTable(
		    std::array<std::pair<InputType, OutputType>, N> const& vals)
		    : m_in_to_out{vals}
		    , m_out_to_in{detail::swap_pairs(vals)}
		{
		}

		static constexpr size_t size() { return N; }

		constexpr OutputType output(InputType x) const { return m_in_to_out(x); }

		constexpr InputType input(OutputType y) const { return m_out_to_in(y); }

	private:
		InterpolationTable<InputType, OutputType, N> m_in_to_out;
		InterpolationTable<OutputType, InputType, N> m_out_to_in;
	};
}

#endif