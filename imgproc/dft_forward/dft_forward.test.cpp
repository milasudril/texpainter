//@	{
//@	 "targets":[{"name":"dft_forward.test", "type":"application", "autorun":1}]
//@	}

#include "./dft_forward.hpp"

#include "filtergraph/image_processor.hpp"

#include <cassert>

static_assert(Texpainter::FilterGraph::ImageProcessor<DftForward::ImageProcessor>);

namespace Testcases
{
	void dftForwardImageProcessorInterfaceDescriptor()
	{
		static_assert(DftForward::ImageProcessor::InterfaceDescriptor::InputPorts.size() == 1);
		static_assert(DftForward::ImageProcessor::InterfaceDescriptor::OutputPorts.size() == 1);

		static_assert(DftForward::ImageProcessor::InterfaceDescriptor::OutputPorts[0].type
		              == DftForward::PixelType::GrayscaleComplex);

		static_assert(DftForward::ImageProcessor::InterfaceDescriptor::InputPorts[0].type
		              == DftForward::PixelType::GrayscaleReal);
	}

	void dftForwardImageProcessorName()
	{
		static_assert(DftForward::ImageProcessor::name() != nullptr);
		static_assert(*DftForward::ImageProcessor::name() != '\0');
	}
}

int main()
{
	Testcases::dftForwardImageProcessorInterfaceDescriptor();
	Testcases::dftForwardImageProcessorName();
	return 0;
}