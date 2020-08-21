//@	{
//@	 "targets":[{"name":"rgba_combine.o", "type":"object"}]
//@	}

#include "./rgba_combine.hpp"

void RgbaCombine::ImageProcessor::operator()(ImgProcArg<InterfaceDescriptor> const& args) const
{
	auto const size = args.size();
	for(uint32_t row = 0; row < size.height(); ++row)
	{
		for(uint32_t col = 0; col < size.width(); ++col)
		{
			args.output<0>(col, row) =
			    RgbaCombine::RgbaValue{static_cast<float>(args.input<0>(col, row)),
			                           static_cast<float>(args.input<1>(col, row)),
			                           static_cast<float>(args.input<2>(col, row)),
			                           static_cast<float>(args.input<3>(col, row))};
		}
	}
}
