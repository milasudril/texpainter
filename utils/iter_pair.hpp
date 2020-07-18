//@	{"targets":[{"name":"iter_pair.hpp", "type":"include"}]}

#ifndef TEXPAINTER_ITERPAIR_HPP
#define TEXPAINTER_ITERPAIR_HPP

#include <ranges>

namespace Texpainter
{
	template<class IterType>
	class IterPair
	{
	public:
		explicit IterPair(IterType begin, IterType end): m_begin{begin}, m_end{end}
		{
		}

		IterType begin() const
		{
			return m_begin;
		}

		IterType end() const
		{
			return m_end;
		}

	private:
		IterType m_begin;
		IterType m_end;
	};
}

namespace std::ranges
{
	template<class IterType>
	inline constexpr bool enable_borrowed_range<Texpainter::IterPair<IterType>> = true;
};


#endif