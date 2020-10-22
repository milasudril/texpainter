//@	{
//@	 "targets":[{"name":"dft_backward.test", "type":"application", "autorun":1}]
//@	}

#include "./dft_backward.hpp"

#include "filtergraph/image_processor.hpp"

#include <cassert>

static_assert(Texpainter::FilterGraph::ImageProcessor<DftBackward::ImageProcessor>);

namespace Testcases
{
	void dftBackwardImageProcessorInterfaceDescriptor()
	{
		static_assert(DftBackward::ImageProcessor::InterfaceDescriptor::InputPorts.size() == 1);
		static_assert(DftBackward::ImageProcessor::InterfaceDescriptor::OutputPorts.size() == 1);

		static_assert(DftBackward::ImageProcessor::InterfaceDescriptor::OutputPorts[0].type
		              == DftBackward::PortType::GrayscaleRealPixels);

		static_assert(DftBackward::ImageProcessor::InterfaceDescriptor::InputPorts[0].type
		              == DftBackward::PortType::GrayscaleComplexPixels);
	}

	void dftBackwardImageProcessorName()
	{
		static_assert(DftBackward::ImageProcessor::name() != nullptr);
		static_assert(*DftBackward::ImageProcessor::name() != '\0');
	}
}

int main()
{
	Testcases::dftBackwardImageProcessorInterfaceDescriptor();
	Testcases::dftBackwardImageProcessorName();
	return 0;
}