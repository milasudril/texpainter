//@	{
//@	 "targets":[{"name":"rgba_split.o", "type":"object"}]
//@	}

#include "./rgba_split.hpp"

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
