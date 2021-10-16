//@	{
//@	 "targets":[{"name":"img_proc_param.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_IMGPROCPARAM_HPP
#define TEXPAINTER_FILTERGRAPH_IMGPROCPARAM_HPP

#include "utils/exponent.hpp"
#include "scaling_factors/scaling_factors.hpp"

#include <compare>
#include <cstring>

namespace Texpainter::FilterGraph
{
	class ParamValue
	{
	public:
		explicit constexpr ParamValue() = default;

		explicit constexpr ParamValue(float val): m_value{val} {}

		constexpr auto operator<=>(ParamValue const&) const = default;

		constexpr auto value() const { return m_value; }

	private:
		float m_value;
	};

	constexpr auto sizeScaleFactor(ParamValue val)
	{
		return static_cast<float>(ScalingFactors::sizeScaleFactor(val.value()));
	}

	constexpr auto sizeFromGeomMean(Size2d size, ParamValue val)
	{
		return static_cast<float>(ScalingFactors::sizeFromGeomMean(size, val.value()));
	}

	constexpr auto sizeFromMin(Size2d size, ParamValue val)
	{
		return static_cast<float>(ScalingFactors::sizeFromMin(size, val.value()));
	}

	constexpr auto sizeFromMax(Size2d size, ParamValue val)
	{
		return static_cast<float>(ScalingFactors::sizeFromMax(size, val.value()));
	}

	constexpr auto sizeFromWidth(Size2d size, ParamValue val)
	{
		return static_cast<float>(ScalingFactors::sizeFromWidth(size, val.value()));
	}

	constexpr auto sizeFromHeight(Size2d size, ParamValue val)
	{
		return static_cast<float>(ScalingFactors::sizeFromHeight(size, val.value()));
	}

	constexpr auto sizeFromArea(Size2d size, ParamValue val)
	{
		return static_cast<float>(ScalingFactors::sizeFromArea(size, val.value()));
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