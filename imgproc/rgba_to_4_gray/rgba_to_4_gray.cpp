//@	{
//@	 "targets":[{"name":"rgba_to_4_gray.o", "type":"object"}]
//@	}

#include "./rgba_to_4_gray.hpp"

std::vector<RgbaTo4Gray::RetVal> RgbaTo4Gray::ImageProcessor::operator()(
    std::span<Arg const> args) const
{
	if(auto src = std::get_if<Texpainter::Span2d<Texpainter::PixelStore::Pixel const>>(args.data());
	   src != nullptr)
	{
		auto size = src->size();
		Texpainter::PixelStore::BasicImage<double> red{size};
		Texpainter::PixelStore::BasicImage<double> green{size};
		Texpainter::PixelStore::BasicImage<double> blue{size};
		Texpainter::PixelStore::BasicImage<double> alpha{size};

		for(uint32_t row = 0; row < size.height(); ++row)
		{
			for(uint32_t col = 0; col < size.width(); ++col)
			{
				auto val        = (*src)(col, row);
				red(col, row)   = val.red();
				green(col, row) = val.green();
				blue(col, row)  = val.blue();
				alpha(col, row) = val.alpha();
			}
		}

		return std::vector<RgbaTo4Gray::RetVal>{
		    std::move(red), std::move(green), std::move(blue), std::move(alpha)};
	}
	return std::vector<RgbaTo4Gray::RetVal>{};
}
