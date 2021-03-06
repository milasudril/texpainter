//@	{"targets":[{"name":"image.hpp", "type":"include"}]}

#ifndef TEXPAINTER_PIXELSTORE_IMAGE_HPP
#define TEXPAINTER_PIXELSTORE_IMAGE_HPP

#include "./pixel.hpp"

#include "utils/span_2d.hpp"

#include <algorithm>
#include <span>
#include <memory>

namespace Texpainter::PixelStore
{
	template<class PixelType>
	class BasicImage
	{
	public:
		explicit BasicImage(Span2d<PixelType const> src): BasicImage{src.size()}
		{
			std::ranges::copy(src, m_data.get());
		}

		explicit BasicImage(uint32_t width, uint32_t height): BasicImage{Size2d{width, height}} {}

		explicit BasicImage(Size2d size)
		    : m_size{size}
		    , m_data{std::make_unique<PixelType[]>(area(size))}
		{
		}

		BasicImage(BasicImage const& src): BasicImage{src.pixels()} {}

		BasicImage& operator=(BasicImage&&) = default;

		BasicImage(BasicImage&&) = default;

		BasicImage& operator=(BasicImage const&) = delete;

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
	inline auto size(BasicImage<PixelType> const& img)
	{
		return img.size();
	}

	template<class PixelType>
	inline auto area(BasicImage<PixelType> const& img)
	{
		return area(img.size());
	}

	using Image = BasicImage<Pixel>;
}

#endif