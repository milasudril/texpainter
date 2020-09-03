//@	{
//@	 "targets":[{"name":"dft_backward.o", "type":"object"}]
//@	}

#include "./dft_backward.hpp"

using TempBuffer = Texpainter::PixelStore::BasicImage<DftBackward::ComplexValue>;

void DftBackward::ImageProcessor::operator()(ImgProcArg<InterfaceDescriptor> const& args) const
{
	auto const size = args.size();
	TempBuffer output_buffer{size};
	r_engine->run<Dft::Direction::Backward>(size, args.input<0>(), output_buffer.pixels().data());

	auto sign_row = 1;
	for(uint32_t row = 0; row < size.height(); ++row)
	{
		auto sign_col = 1;
		for(uint32_t col = 0; col < size.width(); ++col)
		{
			args.output<0>(col, row) = output_buffer(col, row).real() * sign_col * sign_row;
			sign_col *= -1;
		}
		sign_row *= -1;
	}
}