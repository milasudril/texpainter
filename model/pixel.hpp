//@	{"targets":[{"name":"pixel.hpp", "type":"include"}]}

#ifndef TEXPAINTER_MODEL_PIXEL_HPP
#define TEXPAINTER_MODEL_PIXEL_HPP

#include "utils/empty.hpp"
#include "utils/vec4.hpp"

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

	template<class OutputStream>
	void write(std::vector<Pixel> const& pixels, OutputStream stream)
	{
		write(stream, reinterpret_cast<float const*>(pixels.data()), 4*pixels.size());
	}

	template<class InputStream>
	std::vector<Pixel> read(Empty<std::vector<Pixel>>, InputStream stream)
	{
		auto n = read(Empty<size_t>{}, stream);
		std::vector<Pixel> ret(n);
		read(stream, reinterpret_cast<float*>(ret.data()), 4*ret.size());
		return ret;
	}
}
#endif