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
	std::transform(args.input<0>(),
	               args.input<0>() + size,
	               args.output<0>(),
	               [min_in  = *range.first,
	                max_in  = *range.second,
	                min_out = static_cast<float>(m_params.find<Str{"Min"}>()->value()),
	                max_out = static_cast<float>(m_params.find<Str{"Max"}>()->value())](auto val) {
		               auto const t = (val - min_in) / (max_in - min_in);
		               return t * max_out + (1.0f - t) * min_out;
	               });
}