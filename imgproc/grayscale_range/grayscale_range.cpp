//@	{
//@	 "targets":[{"name":"grayscale_range.o", "type":"object"}]
//@	}

#include "./grayscale_range.hpp"

#include <algorithm>

void GrayscaleRange::ImageProcessor::operator()(ImgProcArg<InterfaceDescriptor> const& args) const
{
	auto const size  = args.size().area();
	auto const range = std::minmax_element(args.input<0>(), args.input<0>() + size);

	args.output<0>().get() = *range.first;
	args.output<1>().get() = *range.second;
}