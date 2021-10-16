//@	{"targets":[{"name":"rgba_value.hpp", "type":"include"}]}

#ifndef TEXPAINTER_PIXELTYPES_RGBAVALUE_HPP
#define TEXPAINTER_PIXELTYPES_RGBAVALUE_HPP

#include "utils/vec_t.hpp"

#include "libenum/empty.hpp"

#include <type_traits>
#include <cmath>
#include <algorithm>

namespace Texpainter::PixelTypes
{
	namespace ColorProfiles
	{
		struct LinearRgba
		{
			static constexpr auto toLinear(vec4_t a) { return a; }

			static constexpr auto fromLinear(vec4_t a) { return a; }
		};

		struct Gamma22
		{
			static constexpr vec4_t toLinear(vec4_t val)
			{
				return vec4_t{
				    std::pow(val[0], 2.2f), std::pow(val[1], 2.2f), std::pow(val[2], 2.2f), val[3]};
			}

			static constexpr vec4_t fromLinear(vec4_t val)
			{
				return vec4_t{std::pow(val[0], 1.0f / 2.2f),
				              std::pow(val[1], 1.0f / 2.2f),
				              std::pow(val[2], 1.0f / 2.2f),
				              val[3]};
			}
		};
	}

	template<class ColorProfile>
	class BasicRgbaValue
	{
	public:
		constexpr explicit BasicRgbaValue(vec4_t value): m_value{value} {}

		template<class ColorProfileOther,
		         std::enable_if_t<!std::is_same_v<ColorProfile, ColorProfileOther>, int> = 0>
		constexpr explicit BasicRgbaValue(BasicRgbaValue<ColorProfileOther> x)
		    : m_value{ColorProfile::fromLinear(toLinear(x).value())}
		{
		}

		constexpr explicit BasicRgbaValue(float r, float g, float b, float a = 1.0f)
		    : m_value{r, g, b, a}
		{
		}

		constexpr BasicRgbaValue() = default;

		constexpr BasicRgbaValue(BasicRgbaValue const&) = default;

		constexpr BasicRgbaValue& red(float val)
		{
			m_value[0] = val;
			return *this;
		}

		constexpr BasicRgbaValue& green(float val)
		{
			m_value[1] = val;
			return *this;
		}

		constexpr BasicRgbaValue& blue(float val)
		{
			m_value[2] = val;
			return *this;
		}

		constexpr BasicRgbaValue& alpha(float val)
		{
			m_value[3] = val;
			return *this;
		}

		constexpr BasicRgbaValue& value(vec4_t val) { return *this; }


		constexpr float red() const { return m_value[0]; }

		constexpr float green() const { return m_value[1]; }

		constexpr float blue() const { return m_value[2]; }

		constexpr float alpha() const { return m_value[3]; }


		constexpr auto value() const { return m_value; }

		constexpr BasicRgbaValue& operator+=(BasicRgbaValue other)
		{
			static_assert(std::is_same_v<ColorProfiles::LinearRgba, ColorProfile>);
			m_value += other.m_value;
			return *this;
		}

		constexpr BasicRgbaValue& operator-=(BasicRgbaValue other)
		{
			static_assert(std::is_same_v<ColorProfiles::LinearRgba, ColorProfile>);
			m_value -= other.m_value;
			return *this;
		}

		constexpr BasicRgbaValue& operator/=(BasicRgbaValue other)
		{
			static_assert(std::is_same_v<ColorProfiles::LinearRgba, ColorProfile>);
			m_value /= other.m_value;
			return *this;
		}

		constexpr BasicRgbaValue& operator*=(BasicRgbaValue other)
		{
			static_assert(std::is_same_v<ColorProfiles::LinearRgba, ColorProfile>);
			m_value *= other.m_value;
			return *this;
		}

		constexpr BasicRgbaValue& operator*=(float factor)
		{
			static_assert(std::is_same_v<ColorProfiles::LinearRgba, ColorProfile>);
			m_value *= factor;
			return *this;
		}

		constexpr BasicRgbaValue& operator/=(float factor)
		{
			static_assert(std::is_same_v<ColorProfiles::LinearRgba, ColorProfile>);
			m_value /= factor;
			return *this;
		}

	private:
		vec4_t m_value;
	};

	template<class ColorProfile>
	constexpr auto toLinear(BasicRgbaValue<ColorProfile> a)
	{
		return BasicRgbaValue<ColorProfiles::LinearRgba>{ColorProfile::toLinear(a.value())};
	}

	template<class ColorProfileA, class ColorProfileB>
	constexpr auto operator+(BasicRgbaValue<ColorProfileA> a, BasicRgbaValue<ColorProfileB> b)
	{
		return a += b;
	}

	template<class ColorProfileA, class ColorProfileB>
	constexpr auto operator-(BasicRgbaValue<ColorProfileA> a, BasicRgbaValue<ColorProfileB> b)
	{
		return a -= b;
	}

	template<class ColorProfileA, class ColorProfileB>
	constexpr auto operator*(BasicRgbaValue<ColorProfileA> a, BasicRgbaValue<ColorProfileB> b)
	{
		return a *= b;
	}

	template<class ColorProfileA, class ColorProfileB>
	constexpr auto operator/(BasicRgbaValue<ColorProfileA> a, BasicRgbaValue<ColorProfileB> b)
	{
		return a /= b;
	}

	template<class ColorProfile>
	constexpr auto operator/(BasicRgbaValue<ColorProfile> a, float c)
	{
		return a /= c;
	}

	template<class ColorProfile>
	constexpr auto operator*(BasicRgbaValue<ColorProfile> a, float c)
	{
		return a *= c;
	}

	template<class ColorProfile>
	constexpr auto operator*(float c, BasicRgbaValue<ColorProfile> a)
	{
		return a * c;
	}

	using RgbaValue = BasicRgbaValue<ColorProfiles::LinearRgba>;

	constexpr RgbaValue black() { return RgbaValue{0.0f, 0.0f, 0.0f}; }

	constexpr RgbaValue red() { return RgbaValue{1.0f, 0.0f, 0.0f}; }

	constexpr RgbaValue green() { return RgbaValue{0.0f, 1.0f, 0.0f}; }

	constexpr RgbaValue blue() { return RgbaValue{0.0f, 0.0f, 1.0f}; }

	constexpr RgbaValue cyan() { return RgbaValue{0.0f, 1.0f, 1.0f}; }

	constexpr RgbaValue magenta() { return RgbaValue{1.0f, 0.0f, 1.0f}; }

	constexpr RgbaValue yellow() { return RgbaValue{1.0f, 1.0f, 0.0f}; }

	constexpr RgbaValue white() { return RgbaValue{1.0f, 1.0f, 1.0f}; }

	template<class ColorProfileA, class ColorProfileB>
	constexpr auto distanceSquared(BasicRgbaValue<ColorProfileA> a, BasicRgbaValue<ColorProfileB> b)
	{
		auto tmp = a - b;
		tmp *= tmp;
		return tmp.red() + tmp.green() + tmp.blue() + tmp.alpha();
	}

	template<class ColorProfile>
	constexpr auto absDiff(BasicRgbaValue<ColorProfile> a, BasicRgbaValue<ColorProfile> b)
	{
		auto tmp = a - b;
		return BasicRgbaValue<ColorProfile>{abs(tmp.value())};
	}

	template<class ColorProfile>
	constexpr auto intensity(BasicRgbaValue<ColorProfile> a)
	{
		return a.red() + a.green() + a.blue();
	}

	template<class ColorProfile>
	constexpr auto max(BasicRgbaValue<ColorProfile> a)
	{
		return std::max(a.red(), std::max(a.green(), a.blue()));
	}

	template<class ColorProfile>
	constexpr auto min(BasicRgbaValue<ColorProfile> a)
	{
		return std::min(a.red(), std::min(a.green(), a.blue()));
	}

	inline std::string toString(RgbaValue val) { return Texpainter::toString(val.value()); }

	template<class ColorProfile>
	inline auto fromString(Enum::Empty<BasicRgbaValue<ColorProfile>>, std::string const& str)
	{
		return BasicRgbaValue<ColorProfile>{Texpainter::fromString(Enum::Empty<vec4_t>{}, str)};
	}
}
#endif