//@	{
//@	 "targets":[{"name":"rgba_combine.test", "type":"application", "autorun":1}]
//@	}

#include "./rgba_combine.hpp"

#include "filtergraph/image_processor.hpp"

#include <cassert>

static_assert(Texpainter::FilterGraph::ImageProcessor<RgbaCombine::ImageProcessor>);

namespace Testcases
{
	void rgbaCombineImageProcessorInterfaceDescriptor()
	{
		static_assert(RgbaCombine::ImageProcessor::InterfaceDescriptor::InputPorts.size() == 4);
		static_assert(RgbaCombine::ImageProcessor::InterfaceDescriptor::OutputPorts.size() == 1);

		static_assert(RgbaCombine::ImageProcessor::InterfaceDescriptor::OutputPorts[0].type
		              == RgbaCombine::PixelType::RGBA);

		static_assert(RgbaCombine::ImageProcessor::InterfaceDescriptor::InputPorts[0].type
		              == RgbaCombine::PixelType::GrayscaleReal);
		static_assert(RgbaCombine::ImageProcessor::InterfaceDescriptor::InputPorts[1].type
		              == RgbaCombine::PixelType::GrayscaleReal);
		static_assert(RgbaCombine::ImageProcessor::InterfaceDescriptor::InputPorts[2].type
		              == RgbaCombine::PixelType::GrayscaleReal);
		static_assert(RgbaCombine::ImageProcessor::InterfaceDescriptor::InputPorts[3].type
		              == RgbaCombine::PixelType::GrayscaleReal);
	}
	void rgbaCombineImageProcessorCall()
	{
		std::array<RgbaCombine::RealValue, 6> const red{1.0, 1.0, 0.0, 0.0, 0.0, 1.0};
		std::array<RgbaCombine::RealValue, 6> const green{0.0, 1.0, 1.0, 1.0, 0.0, 0.0};
		std::array<RgbaCombine::RealValue, 6> const blue{0.0, 0.0, 0.0, 1.0, 1.0, 1.0};
		std::array<RgbaCombine::RealValue, 6> const alpha{1.0, 1.0, 1.0, 1.0, 1.0, 1.0};

		std::array<RgbaCombine::RgbaValue, 6> const pixels_out_expected{
		    RgbaCombine::RgbaValue{1.0f, 0.0f, 0.0f, 1.0f},
		    RgbaCombine::RgbaValue{1.0f, 1.0f, 0.0f, 1.0f},
		    RgbaCombine::RgbaValue{0.0f, 1.0f, 0.0f, 1.0f},
		    RgbaCombine::RgbaValue{0.0f, 1.0f, 1.0f, 1.0f},
		    RgbaCombine::RgbaValue{0.0f, 0.0f, 1.0, 1.0f},
		    RgbaCombine::RgbaValue{1.0f, 0.0f, 1.0, 1.0f}};

		using InterfaceDescriptor = RgbaCombine::ImageProcessor::InterfaceDescriptor;
		using ImgProcArg          = RgbaCombine::ImgProcArg<InterfaceDescriptor>;
		using InputArgs           = ImgProcArg::InputArgs;
		using OutputArgs          = ImgProcArg::OutputArgs;

		std::array<RgbaCombine::RgbaValue, 6> pixels_out{};
		InputArgs in{};
		in.get<0>() = red.data();
		in.get<1>() = green.data();
		in.get<2>() = blue.data();
		in.get<3>() = alpha.data();

		OutputArgs out{};
		out.get<0>() = pixels_out.data();

		RgbaCombine::ImageProcessor proc;
		Texpainter::Size2d size{3, 2};
		proc(ImgProcArg{size, in, out});

		assert(std::ranges::equal(pixels_out_expected, pixels_out, [](auto a, auto b) {
			auto diff = a - b;
			return diff.red() == 0.0f && diff.green() == 0.0f && diff.blue() == 0.0f
			       && diff.alpha() == 0.0f;
		}));
	}

	void rgbaCombineImageProcessorName()
	{
		static_assert(RgbaCombine::ImageProcessor::name() != nullptr);
		static_assert(*RgbaCombine::ImageProcessor::name() != '\0');
	}
}

int main()
{
	Testcases::rgbaCombineImageProcessorInterfaceDescriptor();
	Testcases::rgbaCombineImageProcessorCall();
	Testcases::rgbaCombineImageProcessorName();
	return 0;
}