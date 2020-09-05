//@	{
//@	 "targets":[{"name":"grayscale_noise.o", "type":"object"}]
//@	}

#include "./grayscale_noise.hpp"

#include "utils/default_rng.hpp"

#include <algorithm>
#include <random>

namespace DefaultRng = Texpainter::DefaultRng;

void GrayscaleNoise::ImageProcessor::operator()(ImgProcArg<InterfaceDescriptor> const& args) const
{
	auto const size = args.size().area();
	std::generate(args.output<0>(),
	              args.output<0>() + size,
	              [U   = std::uniform_real_distribution{0.0, 1.0},
	               rng = DefaultRng::engine()]() mutable { return U(rng); });
}