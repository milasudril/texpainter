//@	{
//@	 "targets":[{"name":"map_grayscale_range.o", "type":"object"}]
//@	}

#include "./map_grayscale_range.hpp"

#include <algorithm>

void MapGrayscaleRange::ImageProcessor::operator()(
    ImgProcArg<InterfaceDescriptor> const& args) const
{
	auto const size = args.size().area();

	std::transform(args.input<0>(),
	               args.input<0>() + size,
	               args.output<0>(),
	               [min_in = args.input<1>(), max_in = args.input<2>()](auto val) {
		               return (val - min_in) / (max_in - min_in);
	               });
}