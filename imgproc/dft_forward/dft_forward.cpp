//@	{
//@	 "targets":[{"name":"dft_forward.o", "type":"object"}]
//@	}

#include "./dft_forward.hpp"

#include "pixel_store/image.hpp"
#include "dft/engine.hpp"

using TempBuffer = Texpainter::PixelStore::BasicImage<DftForward::ComplexValue>;
namespace Dft    = Texpainter::Dft;

void DftForward::ImageProcessor::operator()(ImgProcArg<InterfaceDescriptor> const& args) const
{
	auto sign_row   = 1;
	auto const size = args.size();
	TempBuffer input_buffer{size};
	for(uint32_t row = 0; row < size.height(); ++row)
	{
		auto sign_col = 1;
		for(uint32_t col = 0; col < size.width(); ++col)
		{
			input_buffer(col, row) = ComplexValue{args.input<0>(col, row) * sign_row * sign_col};
			sign_col *= -1;
		}
		sign_row *= -1;
	}

	auto const output = args.output<0>();
	auto& engine      = Dft::engineInstance();
	engine.run<Dft::Direction::Forward>(size, input_buffer.pixels().data(), output);
	auto const A = size.area();
	std::for_each(output, output + A, [A](auto& val) { return val /= A; });
}
