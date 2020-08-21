//@	{
//@	 "targets":[{"name":"rgba_split.o", "type":"object"}]
//@	}

#include "./rgba_split.hpp"

std::vector<RgbaSplit::RetVal> RgbaSplit::ImageProcessor::operator()(
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

		return std::vector<RgbaSplit::RetVal>{
		    std::move(red), std::move(green), std::move(blue), std::move(alpha)};
	}
	return std::vector<RgbaSplit::RetVal>{};
}

void RgbaSplit::ImageProcessor::operator()(ImgProcArg<InterfaceDescriptor> const& arg) const
{
	auto size = arg.size();
	for(uint32_t row = 0; row < size.height(); ++row)
	{
		for(uint32_t col = 0; col < size.width(); ++col)
		{
			arg.output<0>(col, row) = arg.input<0>(col, row).red();
			arg.output<1>(col, row) = arg.input<0>(col, row).green();
			arg.output<2>(col, row) = arg.input<0>(col, row).blue();
			arg.output<3>(col, row) = arg.input<0>(col, row).alpha();
		}
	}
}
