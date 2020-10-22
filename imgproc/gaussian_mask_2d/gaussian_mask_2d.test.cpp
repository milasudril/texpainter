//@	{
//@	 "targets":[{"name":"gaussian_mask.test", "type":"application", "autorun":1}]
//@	}

#include "./gaussian_mask_2d.hpp"

#include "filtergraph/image_processor.hpp"

#include <cassert>

static_assert(Texpainter::FilterGraph::ImageProcessor<GaussianMask2d::ImageProcessor>);

namespace Testcases
{
	void gaussianMaskImageProcessorInterfaceDescriptor()
	{
		static_assert(GaussianMask2d::ImageProcessor::InterfaceDescriptor::InputPorts.size() == 0);
		static_assert(GaussianMask2d::ImageProcessor::InterfaceDescriptor::OutputPorts.size() == 1);

		static_assert(GaussianMask2d::ImageProcessor::InterfaceDescriptor::OutputPorts[0].type
		              == GaussianMask2d::PortType::GrayscaleRealPixels);
	}

	void gaussianMaskImageProcessorName()
	{
		static_assert(GaussianMask2d::ImageProcessor::name() != nullptr);
		static_assert(*GaussianMask2d::ImageProcessor::name() != '\0');
	}
}

int main()
{
	Testcases::gaussianMaskImageProcessorInterfaceDescriptor();
	Testcases::gaussianMaskImageProcessorName();
	return 0;
}