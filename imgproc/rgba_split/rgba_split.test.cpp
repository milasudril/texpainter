//@	{
//@	 "targets":[{"name":"rgba_split.test", "type":"application", "autorun":1}]
//@	}

#include "./rgba_split.hpp"

#include "filtergraph/image_processor.hpp"

#include <cassert>

static_assert(Texpainter::FilterGraph::ImageProcessor2<RgbaSplit::ImageProcessor>);
namespace Testcases
{
#if 0
	void rgbaSplitImageProcessorInterfaceDescriptor()
	{
		static_assert(RgbaSplit::ImageProcessor::InterfaceDescriptor::InputPorts.size() == 4);
		static_assert(RgbaSplit::ImageProcessor::InterfaceDescriptor::OutputPorts.size() == 1);

		static_assert(RgbaSplit::ImageProcessor::InterfaceDescriptor::OutputPorts[0].type
		              == RgbaSplit::PixelType::RGBA);

		static_assert(RgbaSplit::ImageProcessor::InterfaceDescriptor::InputPorts[0].type
		              == RgbaSplit::PixelType::GrayscaleReal);
		static_assert(RgbaSplit::ImageProcessor::InterfaceDescriptor::InputPorts[1].type
		              == RgbaSplit::PixelType::GrayscaleReal);
		static_assert(RgbaSplit::ImageProcessor::InterfaceDescriptor::InputPorts[2].type
		              == RgbaSplit::PixelType::GrayscaleReal);
		static_assert(RgbaSplit::ImageProcessor::InterfaceDescriptor::InputPorts[3].type
		              == RgbaSplit::PixelType::GrayscaleReal);
	}
	void rgbaSplitImageProcessorCall()
	{
		std::array<RgbaSplit::RealValue, 6> const red{1.0, 1.0, 0.0, 0.0, 0.0, 1.0};
		std::array<RgbaSplit::RealValue, 6> const green{0.0, 1.0, 1.0, 1.0, 0.0, 0.0};
		std::array<RgbaSplit::RealValue, 6> const blue{0.0, 0.0, 0.0, 1.0, 1.0, 1.0};
		std::array<RgbaSplit::RealValue, 6> const alpha{1.0, 1.0, 1.0, 1.0, 1.0, 1.0};

		std::array<RgbaSplit::RgbaValue, 6> const pixels_out_expected{
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

		std::array<RgbaSplit::RgbaValue, 6> pixels_out{};
		InputArgs in{};
		in.get<0>() = red.data();
		in.get<1>() = green.data();
		in.get<2>() = blue.data();
		in.get<3>() = alpha.data();

		OutputArgs out{};
		out.get<0>() = pixels_out.data();

		RgbaSplit::ImageProcessor proc;
		Texpainter::Size2d size{3, 2};
		proc(ImgProcArg{size, in, out});

		assert(std::ranges::equal(pixels_out_expected, pixels_out, [](auto a, auto b) {
			auto diff = a - b;
			return diff.red() == 0.0f && diff.green() == 0.0f && diff.blue() == 0.0f
			       && diff.alpha() == 0.0f;
		}));
	}

	void rgbaSplitImageProcessorName()
	{
		static_assert(RgbaSplit::ImageProcessor::name() != nullptr);
		static_assert(*RgbaSplit::ImageProcessor::name() != '\0');
	}
#endif
}

int main()
{
#if 0
	Testcases::rgbaSplitImageProcessorInterfaceDescriptor();
	Testcases::rgbaSplitImageProcessorCall();
	Testcases::rgbaSplitImageProcessorName();
#endif
	return 0;
}