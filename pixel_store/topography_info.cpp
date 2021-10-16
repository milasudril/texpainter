//@	{
//@	 "targets":[{"name":"topography_info.o","type":"object"}]
//@	}

#include "./topography_info.hpp"

#include <memory>

std::unique_ptr<Texpainter::PixelStore::TopographyInfo[]> Texpainter::PixelStore::downsample(
    Texpainter::Size2d size, TopographyInfo const* src, uint32_t scale)
{
	auto ret = std::make_unique<TopographyInfo[]>(area(size));

	if(scale == 1) [[likely]]
		{
			std::copy_n(src, area(size), ret.get());
			return ret;
		}

	auto const w = size.width();
	auto const h = size.height();

	for(uint32_t row = 0; row != h; ++row)
	{
		for(uint32_t col = 0; col != w; ++col)
		{
			vec4_t result{};
			for(uint32_t row_src = 0; row_src != scale; ++row_src)
			{
				for(uint32_t col_src = 0; col_src != scale; ++col_src)
				{
					auto const x = scale * col + col_src;
					auto const y = scale * row + row_src;
					result += src[y * scale * size.width() + x].value();
				}
			}
			result /= static_cast<float>(scale * scale);
			ret[row * w + col] = TopographyInfo{result};
		}
	}

	return ret;
}