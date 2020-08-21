//@	{
//@	 "targets":[{"name":"rgba_split.test", "type":"application", "autorun":1}]
//@	}

#include "./rgba_split.hpp"

#include "filtergraph/image_processor.hpp"

#include <cassert>

static_assert(Texpainter::FilterGraph::ImageProcessor2<RgbaSplit::ImageProcessor>);

namespace Testcases
{
	void rgbaSplitImageProcessorInterfaceDescriptor()
	{
		static_assert(RgbaSplit::ImageProcessor::InterfaceDescriptor::InputPorts.size() == 1);
		static_assert(RgbaSplit::ImageProcessor::InterfaceDescriptor::OutputPorts.size() == 4);

		static_assert(RgbaSplit::ImageProcessor::InterfaceDescriptor::InputPorts[0].type
		              == RgbaSplit::PixelType::RGBA);

		static_assert(RgbaSplit::ImageProcessor::InterfaceDescriptor::OutputPorts[0].type
		              == RgbaSplit::PixelType::GrayscaleReal);
		static_assert(RgbaSplit::ImageProcessor::InterfaceDescriptor::OutputPorts[1].type
		              == RgbaSplit::PixelType::GrayscaleReal);
		static_assert(RgbaSplit::ImageProcessor::InterfaceDescriptor::OutputPorts[2].type
		              == RgbaSplit::PixelType::GrayscaleReal);
		static_assert(RgbaSplit::ImageProcessor::InterfaceDescriptor::OutputPorts[3].type
		              == RgbaSplit::PixelType::GrayscaleReal);
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

		std::array<RgbaSplit::RealValue, 6> red{};
		std::array<RgbaSplit::RealValue, 6> green{};
		std::array<RgbaSplit::RealValue, 6> blue{};
		std::array<RgbaSplit::RealValue, 6> alpha{};

		InputArgs in{};
		in.get<0>() = pixels_in.data();

		OutputArgs out{};
		out.get<0>() = red.data();
		out.get<1>() = green.data();
		out.get<2>() = blue.data();
		out.get<3>() = alpha.data();

		RgbaSplit::ImageProcessor proc;
		Texpainter::Size2d size{3, 2};
		proc(ImgProcArg{size, in, out});

		assert(red_expected == red);
		assert(green_expected == green);
		assert(blue_expected == blue);
		assert(alpha_expected == alpha);
	}
#if 0

	void rgbaSplitImageProcessorName()
	{
		static_assert(RgbaSplit::ImageProcessor::name() != nullptr);
		static_assert(*RgbaSplit::ImageProcessor::name() != '\0');
	}
#endif
}

int main()
{
	Testcases::rgbaSplitImageProcessorInterfaceDescriptor();
#if 0
	Testcases::rgbaSplitImageProcessorCall();
	Testcases::rgbaSplitImageProcessorName();
#endif
	return 0;
}