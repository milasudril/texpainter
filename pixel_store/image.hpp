//@	{"targets":[{"name":"image.hpp", "type":"include"}]}

#ifndef TEXPAINTER_PIXELSTORE_IMAGE_HPP
#define TEXPAINTER_PIXELSTORE_IMAGE_HPP

#include "utils/span_2d.hpp"

#include <algorithm>
#include <span>
#include <memory>

namespace Texpainter::PixelStore
{
	template<class PixelType>
	class Image
	{
	public:
		explicit Image(Span2d<PixelType const> src): Image{src.size()}
		{
			std::ranges::copy(src, m_data.get());
		}

		explicit Image(uint32_t width, uint32_t height): Image{Size2d{width, height}} {}

		explicit Image(Size2d size): m_size{size}, m_data{std::make_unique<PixelType[]>(area(size))}
		{
		}

		Image(Image const& src): Image{src.pixels()} {}

		Image& operator=(Image&&) = default;

		Image(Image&&) = default;

		Image& operator=(Image const&) = delete;

		auto width() const { return m_size.width(); }

		auto height() const { return m_size.height(); }

		Size2d size() const { return Size2d{width(), height()}; }


		PixelType operator()(uint32_t x, uint32_t y) const { return *getAddress(x, y); }

		PixelType& operator()(uint32_t x, uint32_t y)
		{
			return *const_cast<PixelType*>(std::as_const(*this).getAddress(x, y));
		}

		Span2d<PixelType const> pixels() const
		{
			return {reinterpret_cast<PixelType const*>(m_data.get()), size()};
		}

		Span2d<PixelType> pixels() { return {reinterpret_cast<PixelType*>(m_data.get()), size()}; }

		operator Span2d<PixelType>() { return pixels(); }

		operator Span2d<PixelType const>() const { return pixels(); }

	private:
		Size2d m_size;
		std::unique_ptr<PixelType[]> m_data;

		PixelType const* getAddress(uint32_t x, uint32_t y) const
		{
			auto ptr = reinterpret_cast<PixelType const*>(m_data.get());
			return ptr + y * width() + x;
		}
	};

	template<class PixelType>
	inline auto size(Image<PixelType> const& img)
	{
		return img.size();
	}

	template<class PixelType>
	inline auto area(Image<PixelType> const& img)
	{
		return area(img.size());
	}

	template<class PixelType>
	inline auto crop(Span2d<PixelType const> pixels,
	                 Span2dBoundingBox bb,
	                 uint32_t padding_x = 0u,
	                 uint32_t padding_y = 0u)
	{
		if(!bb.valid()) [[unlikely]]
			{
				return Image<PixelType>{1 + padding_x, 1 + padding_y};
			}

		Image<PixelType> ret{bb.width() + padding_x, bb.height() + padding_y};
		std::fill_n(ret.pixels().data(), area(ret), PixelType{});
		for(uint32_t y = 0u; y != bb.height(); ++y)
		{
			for(uint32_t x = 0u; x != bb.width(); ++x)
			{
				ret(x, y) = pixels(x + bb.x_min, y + bb.y_min);
			}
		}

		return ret;
	}
}

#endif