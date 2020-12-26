//@	{
//@	"targets":[{"name":"discrete_pdf.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_UTILS_DISCRETEPDF_HPP
#define TEXPAINTER_UTILS_DISCRETEPDF_HPP

#include <algorithm>
#include <numeric>
#include <array>

namespace Texpainter
{
	template<class ProbType, size_t N>
	requires(N > 0) class DiscretePdf
	{
	public:
		constexpr explicit DiscretePdf(std::array<ProbType, N> const& vals): m_pdf{}
		{
			auto ge_zero = [](auto val) { return val > static_cast<ProbType>(0); };
			auto i_begin = std::find_if(std::begin(vals), std::end(vals), ge_zero);
			auto i_end   = std::find_if(std::rbegin(vals), std::rend(vals), ge_zero).base();

			if(i_begin == std::end(vals)) [[unlikely]]
				{
					std::ranges::fill(m_pdf, static_cast<ProbType>(1));
					std::partial_sum(std::begin(m_pdf), std::end(m_pdf), std::begin(m_pdf));
					std::transform(std::begin(m_pdf),
					               std::end(m_pdf),
					               std::begin(m_pdf),
					               [](auto val) { return val / N; });
					m_first_nonzero = 0;
					m_last_nonzero  = N - 1;
					return;
				}
			std::partial_sum(std::begin(vals), i_end, std::begin(m_pdf));

			m_last_nonzero = i_end - std::begin(vals) - 1;
			std::transform(std::begin(m_pdf),
			               std::end(m_pdf),
			               std::begin(m_pdf),
			               [sum = m_pdf[m_last_nonzero]](auto val) { return val / sum; });

			m_first_nonzero = i_begin - std::begin(vals);
			m_last_nonzero  = i_end - std::begin(vals) - 1;
		}

		constexpr size_t eventIndex(ProbType random_value) const
		{
			auto const i   = std::upper_bound(std::begin(m_pdf) + m_first_nonzero,
                                            std::begin(m_pdf) + m_last_nonzero + 1,
                                            random_value);
			auto const ret = static_cast<size_t>(i - std::begin(m_pdf));
			return std::clamp(ret, m_first_nonzero, m_last_nonzero);
		}

		static constexpr size_t size() { return N; }

	private:
		std::array<ProbType, N> m_pdf;
		size_t m_first_nonzero;
		size_t m_last_nonzero;
	};
}

#endif