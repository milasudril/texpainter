//@	{"targets":[{"name":"image.hpp", "type":"include"}]}

#ifndef TEXPAINTER_IMAGE_HPP
#define TEXPAINTER_IMAGE_HPP

#include "./pixel.hpp"
#include "utils/datablock.hpp"
#include "utils/span_2d.hpp"

#include <algorithm>

namespace Texpainter::Model
{
	template<class PixelType>
	class BasicImage
	{
	public:
		explicit BasicImage(uint32_t width, DataBlock<PixelType>&& block):
		   m_width{width},
		   m_block{std::move(block)}
		{
		}

		explicit BasicImage(uint32_t width, uint32_t height): m_width{width}, m_block{width * height}
		{
		}

		explicit BasicImage(std::tuple<uint32_t, uint32_t> size):
		   m_width{std::get<0>(size)},
		   m_block{std::get<0>(size) * std::get<1>(size)}
		{
		}

		operator DataBlock<PixelType> const&() const
		{
			return m_block;
		}

		auto width() const
		{
			return m_width;
		}

		auto height() const
		{
			return std::size(m_block) / m_width;
		}

		PixelType get(uint32_t x, uint32_t y) const
		{
			return *getAddress(x, y);
		}

		PixelType& get(uint32_t x, uint32_t y)
		{
			return *const_cast<PixelType*>(std::as_const(*this).getAddress(x, y));
		}

		PixelType operator()(uint32_t x, uint32_t y) const
		{
			return *getAddress(x, y);
		}

		PixelType& operator()(uint32_t x, uint32_t y)
		{
			return *const_cast<PixelType*>(std::as_const(*this).getAddress(x, y));
		}

		Span2d<PixelType const> pixels() const
		{
			return {std::data(m_block), width(), height()};
		}

		Span2d<PixelType> pixels()
		{
			return {std::data(m_block), width(), height()};
		}

		size_t area() const
		{
			return std::size(m_block);
		}

		std::tuple<uint32_t, uint32_t> size() const
		{
			return {width(), height()};
		}

	private:
		uint32_t m_width;
		DataBlock<PixelType> m_block;

		PixelType const* getAddress(uint32_t x, uint32_t y) const
		{
			auto ptr = std::begin(m_block);
			return ptr + y * width() + x;
		}
	};

	template<class PixelType, class OutputStream>
	void write(BasicImage<PixelType> const& pal, OutputStream stream)
	{
		write(pal.width(), stream);
		write(static_cast<Texpainter::DataBlock<Texpainter::Model::Pixel> const&>(pal), stream);
	}

	template<class PixelType, class InputStream>
	BasicImage<PixelType> read(Empty<BasicImage<PixelType>>, InputStream stream)
	{
		auto width = read(Empty<uint32_t>{}, stream);
		return BasicImage<PixelType>{width, read(Empty<DataBlock<Pixel>>{}, stream)};
	}

	template<class PixelType>
	inline auto size(BasicImage<PixelType> const& img)
	{
		return std::tuple{img.width(), img.height()};
	}

	using Image = BasicImage<Pixel>;
}

#endif