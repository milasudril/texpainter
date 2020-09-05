//@	{
//@	 "targets":[{"name":"butterworth_mask_2d.test", "type":"application", "autorun":1}]
//@	}

#include "./butterworth_mask_2d.hpp"

#include "filtergraph/image_processor.hpp"

#include <cassert>

static_assert(Texpainter::FilterGraph::ImageProcessor<ButterworthMask2d::ImageProcessor>);

namespace Testcases
{
	void butterworthMask2dImageProcessorInterfaceDescriptor()
	{
		static_assert(ButterworthMask2d::ImageProcessor::InterfaceDescriptor::InputPorts.size()
		              == 0);
		static_assert(ButterworthMask2d::ImageProcessor::InterfaceDescriptor::OutputPorts.size()
		              == 1);

		static_assert(ButterworthMask2d::ImageProcessor::InterfaceDescriptor::OutputPorts[0].type
		              == ButterworthMask2d::PixelType::GrayscaleReal);
	}

	void butterworthMask2dImageProcessorName()
	{
		static_assert(ButterworthMask2d::ImageProcessor::name() != nullptr);
		static_assert(*ButterworthMask2d::ImageProcessor::name() != '\0');
	}
}

int main()
{
	Testcases::butterworthMask2dImageProcessorInterfaceDescriptor();
	Testcases::butterworthMask2dImageProcessorName();
	return 0;
}