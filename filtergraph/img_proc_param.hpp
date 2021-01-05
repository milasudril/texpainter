//@	{
//@	 "targets":[{"name":"img_proc_param.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_IMGPROCPARAM_HPP
#define TEXPAINTER_FILTERGRAPH_IMGPROCPARAM_HPP

#include "utils/exponent.hpp"

#include <compare>
#include <cstring>

namespace Texpainter::FilterGraph
{
	class ParamValue
	{
	public:
		explicit constexpr ParamValue() = default;

		explicit constexpr ParamValue(double val): m_value{val} {}

		constexpr auto operator<=>(ParamValue const&) const = default;

		constexpr double value() const { return m_value; }

	private:
		double m_value;
	};

	constexpr auto MinumSize = Exponent{-16.0};

	constexpr double sizeScaleFactor(ParamValue val)
	{
		return fromExponent(val.value()*MinumSize);
	}

	class ParamName
	{
	public:
		constexpr ParamName(char const* str): r_str{str} {}

		constexpr char const* c_str() const { return r_str; }

		constexpr auto operator<=>(ParamName other) const
		{
			auto res = strcmp(r_str, other.r_str);
			if(res == 0) [[unlikely]]
				{
					return std::strong_ordering::equal;
				}

			if(res < 0) { return std::strong_ordering::less; }

			return std::strong_ordering::greater;
		}

		constexpr bool operator==(ParamName const& other) const = default;

	private:
		char const* r_str;
	};
}

#endif