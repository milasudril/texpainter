//@	{
//@	 "targets":[{"name":"rgba_split.test", "type":"application", "autorun":1}]
//@	}

#include "./rgba_split.hpp"

#include "filtergraph/image_processor.hpp"

#include <cassert>

static_assert(Texpainter::FilterGraph::ImageProcessor<RgbaSplit::ImageProcessor>);

namespace Testcases
{
	void rgbaSplitImageProcessorInterfaceDescriptor()
	{
		static_assert(RgbaSplit::ImageProcessor::InterfaceDescriptor::InputPorts.size() == 1);
		static_assert(RgbaSplit::ImageProcessor::InterfaceDescriptor::OutputPorts.size() == 4);

		static_assert(RgbaSplit::ImageProcessor::InterfaceDescriptor::InputPorts[0].type
		              == RgbaSplit::PortType::RgbaPixels);

		static_assert(RgbaSplit::ImageProcessor::InterfaceDescriptor::OutputPorts[0].type
		              == RgbaSplit::PortType::GrayscaleRealPixels);
		static_assert(RgbaSplit::ImageProcessor::InterfaceDescriptor::OutputPorts[1].type
		              == RgbaSplit::PortType::GrayscaleRealPixels);
		static_assert(RgbaSplit::ImageProcessor::InterfaceDescriptor::OutputPorts[2].type
		              == RgbaSplit::PortType::GrayscaleRealPixels);
		static_assert(RgbaSplit::ImageProcessor::InterfaceDescriptor::OutputPorts[3].type
		              == RgbaSplit::PortType::GrayscaleRealPixels);
	}
	void rgbaSplitImageProcessorCall()
	{
		std::array<RgbaSplit::RealValue, 6> const red_expected{1.0, 1.0, 0.0, 0.0, 0.0, 1.0};
		std::array<RgbaSplit::RealValue, 6> const green_expected{0.0, 1.0, 1.0, 1.0, 0.0, 0.0};
		std::array<RgbaSplit::RealValue, 6> const blue_expected{0.0, 0.0, 0.0, 1.0, 1.0, 1.0};
		std::array<RgbaSplit::RealValue, 6> const alpha_expected{1.0, 1.0, 1.0, 1.0, 1.0, 1.0};

		std::array<RgbaSplit::RgbaValue, 6> const pixels_in{
		    RgbaSplit::RgbaValue{1.0f, 0.0f, 0.0f, 1.0f},
		    RgbaSplit::RgbaValue{1.0f, 1.0f, 0.0f, 1.0f},
		    RgbaSplit::RgbaValue{0.0f, 1.0f, 0.0f, 1.0f},
		    RgbaSplit::RgbaValue{0.0f, 1.0f, 1.0f, 1.0f},
		    RgbaSplit::RgbaValue{0.0f, 0.0f, 1.0, 1.0f},
		    RgbaSplit::RgbaValue{1.0f, 0.0f, 1.0, 1.0f}};

		using InterfaceDescriptor = RgbaSplit::ImageProcessor::InterfaceDescriptor;
		using ImgProcArg          = RgbaSplit::ImgProcArg<InterfaceDescriptor>;
		using InputArgs           = ImgProcArg::InputArgs;
		using OutputArgs          = ImgProcArg::OutputArgs;
		using OutputBuffers =
		    Texpainter::FilterGraph::OutputBuffers<portTypes(InterfaceDescriptor::OutputPorts)>;

		Texpainter::Size2d size{3, 2};

		OutputBuffers outputs{size};
		;

		InputArgs in{};
		in.get<0>() = pixels_in.data();


		RgbaSplit::ImageProcessor proc;
		proc(ImgProcArg{size, in, OutputArgs{outputs}});

		assert(std::equal(
		    std::begin(red_expected), std::end(red_expected), outputs.template get<0>()));
		assert(std::equal(
		    std::begin(green_expected), std::end(green_expected), outputs.template get<1>()));
		assert(std::equal(
		    std::begin(blue_expected), std::end(blue_expected), outputs.template get<2>()));
		assert(std::equal(
		    std::begin(alpha_expected), std::end(alpha_expected), outputs.template get<3>()));
	}

	void rgbaSplitImageProcessorName()
	{
		static_assert(RgbaSplit::ImageProcessor::name() != nullptr);
		static_assert(*RgbaSplit::ImageProcessor::name() != '\0');
	}
}

int main()
{
	Testcases::rgbaSplitImageProcessorInterfaceDescriptor();
	Testcases::rgbaSplitImageProcessorCall();
	Testcases::rgbaSplitImageProcessorName();
	return 0;
}