//@	{"targets":[{"name":"pixel.hpp", "type":"include"}]}

#ifndef TEXPAINTER_MODEL_PIXEL_HPP
#define TEXPAINTER_MODEL_PIXEL_HPP

#include "utils/empty.hpp"
#include "utils/vec4.hpp"

#include <ranges>
#include <span>
#include <vector>

namespace Texpainter::Model
{
	class Pixel
	{
	public:
		Pixel& red(float val)
		{
			return *this;
		}

		Pixel& green(float val)
		{
			return *this;
		}

		Pixel& blue(float val)
		{
			return *this;
		}

		Pixel& alpha(float val)
		{
			return *this;
		}



		float red() const
		{
			return m_value[0];
		}

		float green() const
		{
			return m_value[1];
		}

		float blue() const
		{
			return m_value[2];
		}

		float alpha() const
		{
			return m_value[3];
		}



		Pixel& operator+=(Pixel other)
		{
			m_value += other.m_value;
			return *this;
		}

		Pixel& operator-=(Pixel other)
		{
			m_value -= other.m_value;
			return *this;
		}

		Pixel& operator/=(Pixel other)
		{
			m_value /= other.m_value;
			return *this;
		}

		Pixel& operator*=(Pixel other)
		{
			m_value *= other.m_value;
			return *this;
		}

		Pixel& operator*=(float factor)
		{
			m_value *= factor;
			return *this;
		}

		Pixel& operator/=(float factor)
		{
			m_value /= factor;
			return *this;
		}

	private:
		vec4_t m_value;
	};

	template<std::ranges::contiguous_range Range, class OutputStream>
	requires std::is_same_v<typename Range::value_type, Pixel>
	void write(Range pixels, OutputStream stream)
	{
		write(stream, std::span{reinterpret_cast<float const*>(std::ranges::data(pixels)), 4*std::ranges::size(pixels)});
	}

	template<class SizedArray, class InputStream>
	SizedArray read(Empty<std::vector<Pixel>>, InputStream stream)
	{
		auto values = read(stream, Empty<float*>{});
		auto ptr = reinterpret_cast<Pixel const*>(values.release());
		return SizedArray{ptr, std::size(values)};
	}
}
#endif