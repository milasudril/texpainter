//@	{
//@	 "targets":[{"name":"rgba_combine.test", "type":"application", "autorun":1}]
//@	}

#include "./rgba_combine.hpp"

#include "filtergraph/image_processor.hpp"

static_assert(Texpainter::FilterGraph::ImageProcessor2<RgbaCombine::ImageProcessor>);

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
#if 0
	void rgbaCombineImageProcessorCall()
	{
		std::array<Texpainter::FilterGraph::RealValue, 6> pixels{1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
		auto size = Texpainter::Size2d{3, 2};
		Texpainter::FilterGraph::ImageSource<Texpainter::FilterGraph::RealValue> src{
		    Texpainter::Span2d{pixels.data(), size}};
		using InterfaceDescriptor = Texpainter::FilterGraph::ImageSource<
		    Texpainter::FilterGraph::RealValue>::InterfaceDescriptor;
		using ImgProcArg = Texpainter::FilterGraph::ImgProcArg2<InterfaceDescriptor>;
		using InputArgs  = ImgProcArg::InputArgs;
		using OutputArgs = ImgProcArg::OutputArgs;

		std::array<Texpainter::FilterGraph::RealValue, 6> pixels_out{};
		OutputArgs args{};
		args.template get<0>() = pixels_out.data();
		src(ImgProcArg{size, InputArgs{}, args});
		assert(std::ranges::equal(pixels, pixels_out));
	}
#endif
	void rgbaCombineImageProcessorName()
	{
		static_assert(RgbaCombine::ImageProcessor::name() != nullptr);
		static_assert(*RgbaCombine::ImageProcessor::name() != '\0');
	}
}

int main()
{
	Testcases::rgbaCombineImageProcessorInterfaceDescriptor();
	//	Testcases::rgbaCombineImageProcessorCall();
	Testcases::rgbaCombineImageProcessorName();
	return 0;
}