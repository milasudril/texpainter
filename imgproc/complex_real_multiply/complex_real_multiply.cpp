//@	{
//@	 "targets":[{"name":"complex_real_multiply.o", "type":"object"}]
//@	}

#include "./complex_real_multiply.hpp"

void ComplexRealMultiply::ImageProcessor::operator()(
    ImgProcArg<InterfaceDescriptor> const& args) const
{
	auto const size = args.size();
	for(uint32_t row = 0; row < size.height(); ++row)
	{
		for(uint32_t col = 0; col < size.width(); ++col)
		{
			args.output<0>(col, row) = args.input<0>(col, row)* args.input<1>(col, row);
		}
	}
}
