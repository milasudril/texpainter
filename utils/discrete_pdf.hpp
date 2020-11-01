//@	{
//@	"targets":[{"name":"discrete_pdf.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_UTILS_DISCRETECDF_HPP
#define TEXPAINTER_UTILS_DISCRETECDF_HPP

#include <algorithm>
#include <numeric>
#include <array>

namespace Texpainter
{
	template<class ProbType, size_t N>
	requires(N > 0) class DiscretePdf
	{
	public:
		constexpr explicit DiscretePdf(std::array<ProbType, N> const& vals)
		{
			std::partial_sum(std::begin(vals), std::end(vals), std::begin(m_pdf));
			std::transform(std::begin(m_pdf),
			               std::end(m_pdf),
			               std::begin(m_pdf),
			               [sum = m_pdf[N - 1]](auto val) { return val / sum; });
		}

		constexpr size_t eventIndex(ProbType random_value) const
		{
			auto i = std::ranges::upper_bound(m_pdf, random_value);
			return std::min(N - 1, static_cast<size_t>(i - std::begin(m_pdf)));
		}

		static constexpr size_t size() { return N; }

	private:
		std::array<ProbType, N> m_pdf;
	};
}

#endif