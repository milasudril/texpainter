//@	{
//@	 "targets":[{"name":"gaussian_blur.test", "type":"application", "autorun":1}]
//@	}

#include "./gaussian_blur.hpp"

#include "filtergraph/image_processor.hpp"

#include <cassert>

static_assert(Texpainter::FilterGraph::ImageProcessor<GaussianBlur::ImageProcessor>);

namespace Testcases
{
	void gaussianBlurImageProcessorInterfaceDescriptor()
	{
		static_assert(GaussianBlur::ImageProcessor::InterfaceDescriptor::InputPorts.size() == 1);
		static_assert(GaussianBlur::ImageProcessor::InterfaceDescriptor::OutputPorts.size() == 1);

		static_assert(GaussianBlur::ImageProcessor::InterfaceDescriptor::OutputPorts[0].type
		              == GaussianBlur::PixelType::GrayscaleComplex);

		static_assert(GaussianBlur::ImageProcessor::InterfaceDescriptor::InputPorts[0].type
		              == GaussianBlur::PixelType::GrayscaleComplex);
	}

	void gaussianBlurImageProcessorName()
	{
		static_assert(GaussianBlur::ImageProcessor::name() != nullptr);
		static_assert(*GaussianBlur::ImageProcessor::name() != '\0');
	}
}

int main()
{
	Testcases::gaussianBlurImageProcessorInterfaceDescriptor();
	Testcases::gaussianBlurImageProcessorName();
	return 0;
}