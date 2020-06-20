//@	{"targets":[{"name":"frequency.hpp", "type":"include"}]}

#ifndef TEXPAINTER_UTILS_FREQUENCY_HPP
#define TEXPAINTER_UTILS_FREQUENCY_HPP

namespace Texpainter
{
	class Frequency
	{
		public:
			constexpr explicit Frequency(double val):m_val{val}
			{}

			constexpr double operator/(Frequency other) const
			{
				return m_val/other.m_val;
			}

			constexpr Frequency& operator+=(Frequency other)
			{
				m_val += other.m_val;
				return *this;
			}

			constexpr Frequency& operator-=(Frequency other)
			{
				m_val += other.m_val;
				return *this;
			}

			constexpr Frequency& operator*=(double val)
			{
				m_val *= val;
				return *this;
			}

			constexpr Frequency& operator/=(double val)
			{
				m_val /= val;
				return *this;
			}


		private:
			double m_val;
	};

	constexpr auto operator+(Frequency f1, Frequency f2)
	{
		f1+=f2;
		return f1;
	}

	constexpr auto operator-(Frequency f1, Frequency f2)
	{
		f1-=f2;
		return f1;
	}

	constexpr auto operator*(Frequency f1, double c)
	{
		f1*=c;
		return f1;
	}

	constexpr auto operator/(Frequency f1, double c)
	{
		f1/=c;
		return f1;
	}

	constexpr auto operator*(double c, Frequency f2)
	{
		return f2*c;
	}
}

#endif