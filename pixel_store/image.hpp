//@	{"targets":[{"name":"image.hpp", "type":"include"}]}

#ifndef TEXPAINTER_PIXELSTORE_IMAGE_HPP
#define TEXPAINTER_PIXELSTORE_IMAGE_HPP

#include "./pixel.hpp"

#include "utils/span_2d.hpp"
#include "utils/memblock.hpp"
#include "utils/trivial.hpp"

#include <algorithm>
#include <span>
#include <cstring>

namespace Texpainter::PixelStore
{
	template<Trivial PortType>
	class BasicImage
	{
	public:
		explicit BasicImage(Size2d size, Memblock&& data): m_size{size}, m_data(data.release()) {}

		explicit BasicImage(Span2d<PortType const> src): BasicImage{src.size()}
		{
			memcpy(m_data.get(), src.data(), area() * sizeof(PortType));
		}

		explicit BasicImage(uint32_t width, uint32_t height): BasicImage{Size2d{width, height}} {}

		explicit BasicImage(Size2d size): m_size(size), m_data{area() * sizeof(PortType)} {}

		BasicImage(BasicImage const& src): BasicImage{src.size()}
		{
			memcpy(m_data.get(), src.m_data.get(), area() * sizeof(PortType));
		}

		BasicImage& operator=(BasicImage&&) = default;

		BasicImage(BasicImage&&) = default;

		BasicImage& operator=(BasicImage const&) = delete;

		auto width() const { return m_size.width(); }

		auto height() const { return m_size.height(); }

		auto area() const { return m_size.area(); }

		Size2d size() const { return Size2d{width(), height()}; }


		PortType operator()(uint32_t x, uint32_t y) const { return *getAddress(x, y); }

		PortType& operator()(uint32_t x, uint32_t y)
		{
			return *const_cast<PortType*>(std::as_const(*this).getAddress(x, y));
		}

		Span2d<PortType const> pixels() const
		{
			return {reinterpret_cast<PortType const*>(m_data.get()), size()};
		}

		Span2d<PortType> pixels() { return {reinterpret_cast<PortType*>(m_data.get()), size()}; }

		operator Span2d<PortType>() { return pixels(); }

		operator Span2d<PortType const>() const { return pixels(); }

	private:
		Size2d m_size;
		Memblock m_data;

		PortType const* getAddress(uint32_t x, uint32_t y) const
		{
			auto ptr = reinterpret_cast<PortType const*>(m_data.get());
			return ptr + y * width() + x;
		}
	};

	template<class PortType, class OutputStream>
	void write(BasicImage<PortType> const& img, OutputStream stream)
	{
		write(img.size(), stream);
		write(std::span{img.pixels().begin(), img.area()}, stream);
	}

	template<class PortType, class InputStream>
	BasicImage<PortType> read(Empty<BasicImage<PortType>>, InputStream stream)
	{
		auto size = read(Empty<Size2d>{}, stream);
		BasicImage<PortType> ret{size};
		read(std::span{ret.pixels().begin(), ret.area()}, stream);
		return ret;
	}

	template<class PortType>
	inline auto size(BasicImage<PortType> const& img)
	{
		return img.size();
	}

	using Image = BasicImage<Pixel>;
}

#endif