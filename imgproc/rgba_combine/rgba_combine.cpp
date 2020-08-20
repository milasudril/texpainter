//@	{
//@	 "targets":[{"name":"rgba_combine.o", "type":"object"}]
//@	}

#include "./rgba_combine.hpp"

std::vector<RgbaCombine::RetVal> RgbaCombine::ImageProcessor::operator()(
    std::span<Arg const> args) const
{
	std::array<Texpainter::Span2d<double const> const*, 4> src{};

	auto const n = std::min(std::size(src), std::size(args));
	for(size_t k = 0; k < n; ++k)
	{
		src[k] = std::get_if<Texpainter::Span2d<double const>>(&args[k]);
		if(src[k] == nullptr) { break; }
	}

	if(n != 4) [[unlikely]]
		{
			return std::vector<RgbaCombine::RetVal>{};
		}

	auto size = src[0]->size();
	Texpainter::PixelStore::Image ret{size};

	for(uint32_t row = 0; row < size.height(); ++row)
	{
		for(uint32_t col = 0; col < size.width(); ++col)
		{
			ret(col, row).red((*src[0])(col, row));
			ret(col, row).green((*src[1])(col, row));
			ret(col, row).blue((*src[2])(col, row));
			ret(col, row).alpha((*src[3])(col, row));
		}
	}
	return std::vector<RgbaCombine::RetVal>{std::move(ret)};
}

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
