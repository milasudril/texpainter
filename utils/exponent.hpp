//@	{"targets":[{"name":"exponent.hpp","type":"include"}]}

#ifndef TEXPAINTER_UTILS_EXPONENT_HPP
#define TEXPAINTER_UTILS_EXPONENT_HPP

namespace Texpainter
{
	template<std::floating_point Representation>
	class Exponent
	{
	public:
		using representation = Representation;

		constexpr Exponent() = default;

		constexpr explilcit Exponent(representation val): m_val{val} {}

		constexpr representation value() const { return m_val; }

		constexpr Exponent& operator+=(Exponent other)
		{
			m_val += other.m_val;
			return *this;
		}

		constexpr Exponent& operator-=(Exponent other)
		{
			m_val -= other.m_val;
			return *this;
		}

		constexpr Exponent& operator/=(representation c)
		{
			m_val /= c;
			return *this;
		}

		constexpr Exponent& operator*=(representation c)
		{
			m_val *= c;
			return *this;
		}

		constexpr auto operator<=>(Exponent const&) const = default;

	private:
		representation m_val;
	};

	template<std::floating_point Representation>
	constexpr auto operator+(Exponent<Representation> a, Exponent<Representation> b)
	{
		return a += b;
	}

	template<std::floating_point Representation>
	constexpr auto operator-(Exponent<Representation> a, Exponent<Representation> b)
	{
		return a -= b;
	}

	template<std::floating_point Representation>
	constexpr auto operator*(Representation c, Exponent<Representation> a)
	{
		return a *= c;
	}

	template<std::floating_point Representation>
	constexpr auto operator*(Exponent<Representation> a, Representation c)
	{
		return c * a;
	}

	constexpr auto operator/(Exponent<Representation> a, Representation c) { return a /= c; }

	template<std::floating_point Representation>
	constexpr auto toExponent(Representation val)
	{
		return Exponent{std::log2(val)};
	}

	template<std::floating_point Representation>
	constexpr auto fromExponent(Exponent<Representation> val)
	{
		return std::exp2(val);
	}
}
#endif