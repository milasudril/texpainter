//@	{
//@	 "targets":[{"name":"gaussian_mask.test", "type":"application", "autorun":1}]
//@	}

#include "./gaussian_mask.hpp"

#include "filtergraph/image_processor.hpp"

#include <cassert>

static_assert(Texpainter::FilterGraph::ImageProcessor<GaussianMask::ImageProcessor>);

namespace Testcases
{
	void gaussianMaskImageProcessorInterfaceDescriptor()
	{
		static_assert(GaussianMask::ImageProcessor::InterfaceDescriptor::InputPorts.size() == 0);
		static_assert(GaussianMask::ImageProcessor::InterfaceDescriptor::OutputPorts.size() == 1);

		static_assert(GaussianMask::ImageProcessor::InterfaceDescriptor::OutputPorts[0].type
		              == GaussianMask::PixelType::GrayscaleReal);
	}

	void gaussianMaskImageProcessorName()
	{
		static_assert(GaussianMask::ImageProcessor::name() != nullptr);
		static_assert(*GaussianMask::ImageProcessor::name() != '\0');
	}
}

int main()
{
	Testcases::gaussianMaskImageProcessorInterfaceDescriptor();
	Testcases::gaussianMaskImageProcessorName();
	return 0;
}