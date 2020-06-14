//@	{"targets":[{"name":"pixel.hpp", "type":"include"}]}

#ifndef TEXPAINTER_MODEL_PIXEL_HPP
#define TEXPAINTER_MODEL_PIXEL_HPP

#include "utils/empty.hpp"
#include "utils/vec_t.hpp"

namespace Texpainter::Model
{
	class Pixel
	{
	public:
		constexpr explicit Pixel(float r, float g, float b, float a = 1.0f): m_value{r, g, b, a}
		{
		}

		constexpr Pixel() = default;

		constexpr Pixel& red(float val)
		{
			return *this;
		}

		constexpr Pixel& green(float val)
		{
			return *this;
		}

		constexpr Pixel& blue(float val)
		{
			return *this;
		}

		constexpr Pixel& alpha(float val)
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


		constexpr Pixel& operator+=(Pixel other)
		{
			m_value += other.m_value;
			return *this;
		}

		constexpr Pixel& operator-=(Pixel other)
		{
			m_value -= other.m_value;
			return *this;
		}

		constexpr Pixel& operator/=(Pixel other)
		{
			m_value /= other.m_value;
			return *this;
		}

		constexpr Pixel& operator*=(Pixel other)
		{
			m_value *= other.m_value;
			return *this;
		}

		constexpr Pixel& operator*=(float factor)
		{
			m_value *= factor;
			return *this;
		}

		constexpr Pixel& operator/=(float factor)
		{
			m_value /= factor;
			return *this;
		}

	private:
		vec4_t m_value;
	};

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

	constexpr auto operator+(Pixel a, Pixel b)
	{
		return a += b;
	}

	constexpr auto operator-(Pixel a, Pixel b)
	{
		return a -= b;
	}

	constexpr auto operator*(Pixel a, Pixel b)
	{
		return a *= b;
	}

	constexpr auto operator/(Pixel a, Pixel b)
	{
		return a /= b;
	}

	constexpr auto operator/(Pixel a, float c)
	{
		return a /= c;
	}

	constexpr auto operator*(Pixel a, float c)
	{
		return a *= c;
	}

	constexpr auto operator*(float c, Pixel a)
	{
		return a * c;
	}


	constexpr auto distanceSquared(Pixel a, Pixel b)
	{
		a -= b;
		a *= a;
		return a.red() + a.green() + a.blue() + a.alpha();
	}
}
#endif