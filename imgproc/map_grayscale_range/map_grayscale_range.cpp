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
	               [min_in  = args.input<1>(),
	                max_in  = args.input<2>(),
	                min_out = static_cast<float>(m_params.find<Str{"Min"}>()->value()),
	                max_out = static_cast<float>(m_params.find<Str{"Max"}>()->value())](auto val) {
		               auto const t = (val - min_in) / (max_in - min_in);
		               return t * max_out + (1.0f - t) * min_out;
	               });
}