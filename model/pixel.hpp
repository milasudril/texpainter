//@	{"targets":[{"name":"pixel.hpp", "type":"include"}]}

#ifndef TEXPAINTER_MODEL_PIXEL_HPP
#define TEXPAINTER_MODEL_PIXEL_HPP

#include "utils/empty.hpp"
#include "utils/vec_t.hpp"

namespace Texpainter::Model
{
	template<class ColorProfile>
	class BasicPixel
	{
	public:
		constexpr explicit BasicPixel(float r, float g, float b, float a = 1.0f): m_value{r, g, b, a}
		{
		}

		constexpr BasicPixel() = default;

		constexpr BasicPixel& red(float val)
		{
			m_value[0] = val;
			return *this;
		}

		constexpr BasicPixel& green(float val)
		{
			m_value[1] = val;
			return *this;
		}

		constexpr BasicPixel& blue(float val)
		{
			m_value[2] = val;
			return *this;
		}

		constexpr BasicPixel& alpha(float val)
		{
			m_value[3] = val;
			return *this;
		}

		constexpr BasicPixel& value(vec4_t val)
		{
			return *this;
		}


		constexpr float red() const
		{
			return m_value[0];
		}

		constexpr float green() const
		{
			return m_value[1];
		}

		constexpr float blue() const
		{
			return m_value[2];
		}

		constexpr float alpha() const
		{
			return m_value[3];
		}


		constexpr auto value() const
		{
			return m_value;
		}


		template<class ColorProfileOther>
		constexpr BasicPixel& operator+=(BasicPixel<ColorProfileOther> other)
		{
			m_value = ColorProfile::fromLinear(ColorProfile::toLinear(m_value)
			                                   + ColorProfileOther::toLinear(other.value()));
			return *this;
		}

		template<class ColorProfileOther>
		constexpr BasicPixel& operator-=(BasicPixel<ColorProfileOther> other)
		{
			m_value = ColorProfile::fromLinear(ColorProfile::toLinear(m_value)
			                                   - ColorProfileOther::toLinear(other.value()));
			return *this;
		}

		template<class ColorProfileOther>
		constexpr BasicPixel& operator/=(BasicPixel<ColorProfileOther> other)
		{
			m_value = ColorProfile::fromLinear(ColorProfile::toLinear(m_value)
			                                   / ColorProfileOther::toLinear(other.value()));
			return *this;
		}

		template<class ColorProfileOther>
		constexpr BasicPixel& operator*=(BasicPixel<ColorProfileOther> other)
		{
			m_value = ColorProfile::fromLinear(ColorProfile::toLinear(m_value)
			                                   * ColorProfileOther::toLinear(other.value()));
			return *this;
		}

		constexpr BasicPixel& operator*=(float factor)
		{
			m_value = ColorProfile::fromLinear(factor * ColorProfile::toLinear(m_value));
			return *this;
		}

		constexpr BasicPixel& operator/=(float factor)
		{
			m_value = ColorProfile::fromLinear(ColorProfile::toLinear(m_value) / factor);
			return *this;
		}

	private:
		vec4_t m_value;
	};

	template<class ColorProfileA, class ColorProfileB>
	constexpr auto operator+(BasicPixel<ColorProfileA> a, BasicPixel<ColorProfileB> b)
	{
		return a += b;
	}

	template<class ColorProfileA, class ColorProfileB>
	constexpr auto operator-(BasicPixel<ColorProfileA> a, BasicPixel<ColorProfileB> b)
	{
		return a -= b;
	}

	template<class ColorProfileA, class ColorProfileB>
	constexpr auto operator*(BasicPixel<ColorProfileA> a, BasicPixel<ColorProfileB> b)
	{
		return a *= b;
	}

	template<class ColorProfileA, class ColorProfileB>
	constexpr auto operator/(BasicPixel<ColorProfileA> a, BasicPixel<ColorProfileB> b)
	{
		return a /= b;
	}

	template<class ColorProfile>
	constexpr auto operator/(BasicPixel<ColorProfile> a, float c)
	{
		return a /= c;
	}

	template<class ColorProfile>
	constexpr auto operator*(BasicPixel<ColorProfile> a, float c)
	{
		return a *= c;
	}

	template<class ColorProfile>
	constexpr auto operator*(float c, BasicPixel<ColorProfile> a)
	{
		return a * c;
	}

	struct LinearRGBA
	{
		static constexpr auto toLinear(vec4_t a)
		{
			return a;
		}

		static constexpr auto fromLinear(vec4_t a)
		{
			return a;
		}
	};

	using Pixel = BasicPixel<LinearRGBA>;

	constexpr Pixel black()
	{
		return Pixel{0.0f, 0.0f, 0.0f};
	}

	constexpr Pixel red()
	{
		return Pixel{1.0f, 0.0f, 0.0f};
	}

	constexpr Pixel green()
	{
		return Pixel{0.0f, 1.0f, 0.0f};
	}

	constexpr Pixel blue()
	{
		return Pixel{0.0f, 0.0f, 1.0f};
	}

	constexpr Pixel cyan()
	{
		return Pixel{0.0f, 1.0f, 1.0f};
	}

	constexpr Pixel magenta()
	{
		return Pixel{1.0f, 0.0f, 1.0f};
	}

	constexpr Pixel yellow()
	{
		return Pixel{1.0f, 1.0f, 0.0f};
	}

	constexpr Pixel white()
	{
		return Pixel{1.0f, 1.0f, 1.0f};
	}

	template<class ColorProfileA, class ColorProfileB>
	constexpr auto distanceSquared(BasicPixel<ColorProfileA> a, BasicPixel<ColorProfileB> b)
	{
		auto tmp = a - b;
		tmp *= tmp;
		return tmp.red() + tmp.green() + tmp.blue() + tmp.alpha();
	}
}
#endif