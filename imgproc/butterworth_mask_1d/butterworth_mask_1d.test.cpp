//@	{
//@	 "targets":[{"name":"butterworth_mask_1d.test", "type":"application", "autorun":1}]
//@	}

#include "./butterworth_mask_1d.hpp"

#include "filtergraph/image_processor.hpp"

#include <cassert>

static_assert(Texpainter::FilterGraph::ImageProcessor<ButterworthMask1d::ImageProcessor>);

namespace Testcases
{
	void butterworthMask1dImageProcessorInterfaceDescriptor()
	{
		static_assert(ButterworthMask1d::ImageProcessor::InterfaceDescriptor::InputPorts.size()
		              == 0);
		static_assert(ButterworthMask1d::ImageProcessor::InterfaceDescriptor::OutputPorts.size()
		              == 1);

		static_assert(ButterworthMask1d::ImageProcessor::InterfaceDescriptor::OutputPorts[0].type
		              == ButterworthMask1d::PixelType::GrayscaleReal);
	}

	void butterworthMask1dImageProcessorName()
	{
		static_assert(ButterworthMask1d::ImageProcessor::name() != nullptr);
		static_assert(*ButterworthMask1d::ImageProcessor::name() != '\0');
	}
}

int main()
{
	Testcases::butterworthMask1dImageProcessorInterfaceDescriptor();
	Testcases::butterworthMask1dImageProcessorName();
	return 0;
}