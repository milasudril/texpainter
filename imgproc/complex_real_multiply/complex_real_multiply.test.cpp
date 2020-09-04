//@	{
//@	 "targets":[{"name":"complex_real_multiply.test", "type":"application", "autorun":1}]
//@	}

#include "./complex_real_multiply.hpp"

#include "filtergraph/image_processor.hpp"

#include <cassert>

static_assert(Texpainter::FilterGraph::ImageProcessor<ComplexRealMultiply::ImageProcessor>);

namespace Testcases
{
	void rgbaCombineImageProcessorInterfaceDescriptor()
	{
		static_assert(ComplexRealMultiply::ImageProcessor::InterfaceDescriptor::InputPorts.size()
		              == 2);
		static_assert(ComplexRealMultiply::ImageProcessor::InterfaceDescriptor::OutputPorts.size()
		              == 1);

		static_assert(ComplexRealMultiply::ImageProcessor::InterfaceDescriptor::OutputPorts[0].type
		              == ComplexRealMultiply::PixelType::GrayscaleComplex);

		static_assert(ComplexRealMultiply::ImageProcessor::InterfaceDescriptor::InputPorts[1].type
		              == ComplexRealMultiply::PixelType::GrayscaleReal);
		static_assert(ComplexRealMultiply::ImageProcessor::InterfaceDescriptor::InputPorts[0].type
		              == ComplexRealMultiply::PixelType::GrayscaleComplex);
	}

	void rgbaCombineImageProcessorName()
	{
		static_assert(ComplexRealMultiply::ImageProcessor::name() != nullptr);
		static_assert(*ComplexRealMultiply::ImageProcessor::name() != '\0');
	}
}

int main()
{
	Testcases::rgbaCombineImageProcessorInterfaceDescriptor();
	Testcases::rgbaCombineImageProcessorName();
	return 0;
}