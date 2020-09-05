//@	{
//@	 "targets":[{"name":"normalize_grayscale.o", "type":"object"}]
//@	}

#include "./normalize_grayscale.hpp"

#include <algorithm>

void NormalizeGrayscale::ImageProcessor::operator()(
    ImgProcArg<InterfaceDescriptor> const& args) const
{
	auto const size  = args.size().area();
	auto const range = std::minmax_element(args.input<0>(), args.input<0>() + size);
	std::transform(
	    args.input<0>(),
	    args.input<0>() + size,
	    args.output<0>(),
	    [min = *range.first, max = *range.second](auto val) { return (val - min) / (max - min); });
}