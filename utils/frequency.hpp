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
			constexpr explicit BasicFrequency(Rep val):m_val{val}
			{}

			constexpr Rep operator/(BasicFrequency other) const
			{
				return m_val/other.m_val;
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

			constexpr auto value() const
			{ return m_val; }


		private:
			Rep m_val;
	};

	template<class Rep>
	constexpr auto operator+(BasicFrequency<Rep> f1, BasicFrequency<Rep> f2)
	{
		f1+=f2;
		return f1;
	}

	template<class Rep>
	constexpr auto operator-(BasicFrequency<Rep> f1, BasicFrequency<Rep> f2)
	{
		f1-=f2;
		return f1;
	}

	template<class Rep>
	constexpr auto operator*(BasicFrequency<Rep> f1, Rep c)
	{
		f1*=c;
		return f1;
	}

	template<class Rep>
	constexpr auto operator/(BasicFrequency<Rep> f1, Rep c)
	{
		f1/=c;
		return f1;
	}

	template<class Rep>
	constexpr auto operator*(Rep c, BasicFrequency<Rep> f2)
	{
		return f2*c;
	}

	using Frequency = BasicFrequency<double>;
	using SpatialFrequency = BasicFrequency<vec2_t>;

	constexpr auto ξ(SpatialFrequency f)
	{ return Frequency{f.value()[0]}; }

	constexpr auto η(SpatialFrequency f)
	{ return Frequency{f.value()[1]}; }

	Frequency dot(SpatialFrequency f, vec2_t v)
	{
		f *= v;
		return ξ(f) + η(f);
	}
}

#endif