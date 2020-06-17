//@	{"targets":[{"name":"image.hpp", "type":"include"}]}

#ifndef TEXPAINTER_IMAGE_HPP
#define TEXPAINTER_IMAGE_HPP

#include "./pixel.hpp"
#include "utils/datablock.hpp"

#include <algorithm>

namespace Texpainter::Model
{
	class Image
	{
	public:
		explicit Image(uint32_t width, DataBlock<Pixel>&& block):
		   m_width{width},
		   m_block{std::move(block)}
		{
		}

		explicit Image(uint32_t width, uint32_t height, Pixel color_init = black()):
		   m_width{width},
		   m_block{width * height}
		{
			std::ranges::fill(m_block, color_init);
		}

		operator DataBlock<Pixel> const&() const
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

		Pixel get(uint32_t x, uint32_t y) const
		{
			return *getAddress(x, y);
		}

		Pixel& get(uint32_t x, uint32_t y)
		{
			return *const_cast<Pixel*>(std::as_const(*this).getAddress(x, y));
		}

		Pixel const* pixels() const
		{
			return std::data(m_block);
		}

		Pixel* pixels()
		{
			return std::data(m_block);
		}

	private:
		uint32_t m_width;
		DataBlock<Pixel> m_block;

		Pixel const* getAddress(uint32_t x, uint32_t y) const
		{
			x %= width();
			y %= height();
			auto ptr = std::begin(m_block);
			return ptr + y * width() + x;
		}
	};

	template<class OutputStream>
	void write(Image const& pal, OutputStream stream)
	{
		write(pal.width(), stream);
		write(static_cast<Texpainter::DataBlock<Texpainter::Model::Pixel> const&>(pal), stream);
	}

	template<class InputStream>
	Image read(Empty<Image>, InputStream stream)
	{
		auto width = read(Empty<uint32_t>{}, stream);
		return Image{width, read(Empty<DataBlock<Pixel>>{}, stream)};
	}

	inline auto size(Image const& img)
	{
		return std::tuple{img.width(), img.height()};
	}
}

#endif