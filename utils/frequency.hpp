//@	{"targets":[{"name":"frequency.hpp", "type":"include"}]}

#ifndef TEXPAINTER_UTILS_FREQUENCY_HPP
#define TEXPAINTER_UTILS_FREQUENCY_HPP

#include "./vec_t.hpp"

namespace Texpainter
{
	template<class Rep>
	class BasicFrequency
	{
	public:
		constexpr explicit BasicFrequency(BasicFrequency<double> ξ, BasicFrequency<double> η):
		   BasicFrequency{vec2_t{ξ.value(), η.value()}}
		{
		}

		constexpr explicit BasicFrequency(Rep val): m_val{val}
		{
		}

		constexpr Rep operator/(BasicFrequency other) const
		{
			return m_val / other.m_val;
		}

		constexpr BasicFrequency& operator+=(BasicFrequency other)
		{
			m_val += other.m_val;
			return *this;
		}

		constexpr BasicFrequency& operator-=(BasicFrequency other)
		{
			m_val += other.m_val;
			return *this;
		}

		constexpr BasicFrequency& operator*=(Rep val)
		{
			m_val *= val;
			return *this;
		}

		constexpr BasicFrequency& operator/=(Rep val)
		{
			m_val /= val;
			return *this;
		}

		constexpr auto ξ() const
		{
			return BasicFrequency<double>{m_val[0]};
		}

		constexpr auto η() const
		{
			return BasicFrequency<double>{m_val[1]};
		}

		constexpr auto value() const
		{
			return m_val;
		}


	private:
		Rep m_val;
	};

	template<class Rep>
	inline constexpr auto operator+(BasicFrequency<Rep> f1, BasicFrequency<Rep> f2)
	{
		f1 += f2;
		return f1;
	}

	template<class Rep>
	inline constexpr auto operator-(BasicFrequency<Rep> f1, BasicFrequency<Rep> f2)
	{
		f1 -= f2;
		return f1;
	}

	template<class Rep>
	inline constexpr auto operator*(BasicFrequency<Rep> f1, Rep c)
	{
		f1 *= c;
		return f1;
	}

	template<class Rep>
	inline constexpr auto operator/(BasicFrequency<Rep> f1, Rep c)
	{
		f1 /= c;
		return f1;
	}

	template<class Rep>
	inline constexpr auto operator*(Rep c, BasicFrequency<Rep> f2)
	{
		return f2 * c;
	}

	using Frequency = BasicFrequency<double>;
	using SpatialFrequency = BasicFrequency<vec2_t>;

	inline constexpr Frequency dot(SpatialFrequency f, vec2_t v)
	{
		f *= v;
		return f.ξ() + f.η();
	}

	inline constexpr Frequency dot(vec2_t v, SpatialFrequency f)
	{
		return dot(f, v);
	}


	inline constexpr SpatialFrequency transform(SpatialFrequency f, vec2_t ξ_vec, vec2_t η_vec)
	{
		return SpatialFrequency{dot(f, ξ_vec), dot(f, η_vec)};
	}

	inline constexpr Frequency length(SpatialFrequency f)
	{
		auto tmp = dot(f.value(), f.value());
		return Frequency{sqrt(tmp)};
	}
}

#endif