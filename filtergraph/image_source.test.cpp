//@	{
//@	 "targets":[{"name":"image_source.test", "type":"application", "autorun":1}]
//@	}

#include "./image_source.hpp"

#include "./image_processor.hpp"

#include <cassert>

namespace
{
	static_assert(
	    Texpainter::FilterGraph::ImageProcessor<
	        Texpainter::FilterGraph::ImageSource<Texpainter::FilterGraph::PortType::RGBAPixels>>);
}

namespace Testcases
{
	void texpainterFilterGraphImageSourceInterfaceDescriptor()
	{
		static_assert(Texpainter::FilterGraph::ImageSource<
		                  Texpainter::FilterGraph::PortType::GrayscaleRealPixels>::
		                  InterfaceDescriptor::InputPorts.size()
		              == 0);
		static_assert(Texpainter::FilterGraph::ImageSource<
		                  Texpainter::FilterGraph::PortType::GrayscaleRealPixels>::
		                  InterfaceDescriptor::OutputPorts.size()
		              >= 1);

		static_assert(Texpainter::FilterGraph::ImageSource<
		                  Texpainter::FilterGraph::PortType::GrayscaleRealPixels>::
		                  InterfaceDescriptor::OutputPorts[0]
		                      .type
		              == Texpainter::FilterGraph::PortType::GrayscaleRealPixels);
		static_assert(
		    Texpainter::FilterGraph::ImageSource<
		        Texpainter::FilterGraph::PortType::RGBAPixels>::InterfaceDescriptor::OutputPorts[0]
		        .type
		    == Texpainter::FilterGraph::PortType::RGBAPixels);
		static_assert(Texpainter::FilterGraph::ImageSource<
		                  Texpainter::FilterGraph::PortType::GrayscaleComplexPixels>::
		                  InterfaceDescriptor::OutputPorts[0]
		                      .type
		              == Texpainter::FilterGraph::PortType::GrayscaleComplexPixels);
	}

	void texpainterFilterGraphImageSourceCall()
	{
		std::array<Texpainter::FilterGraph::RealValue, 6> pixels{1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
		auto size = Texpainter::Size2d{3, 2};
		Texpainter::FilterGraph::ImageSource<Texpainter::FilterGraph::PortType::GrayscaleRealPixels>
		    src;
		src.source(Texpainter::Span2d{pixels.data(), size});
		using InterfaceDescriptor = Texpainter::FilterGraph::ImageSource<
		    Texpainter::FilterGraph::PortType::GrayscaleRealPixels>::InterfaceDescriptor;
		using ImgProcArg = Texpainter::FilterGraph::ImgProcArg<InterfaceDescriptor>;
		using InputArgs  = ImgProcArg::InputArgs;
		using OutputArgs = ImgProcArg::OutputArgs;

		std::array<Texpainter::FilterGraph::RealValue, 6> pixels_out{};
		OutputArgs args{};
		args.template get<0>() = pixels_out.data();
		src(ImgProcArg{size, InputArgs{}, args});
		assert(std::ranges::equal(pixels, pixels_out));
	}

	void texpainterFilterGraphImageSourceName()
	{
		static_assert(Texpainter::FilterGraph::ImageSource<
		                  Texpainter::FilterGraph::PortType::GrayscaleRealPixels>::name()
		              != nullptr);
		static_assert(*Texpainter::FilterGraph::ImageSource<
		                  Texpainter::FilterGraph::PortType::GrayscaleRealPixels>::name()
		              != '\0');
	}
}

int main()
{
	Testcases::texpainterFilterGraphImageSourceInterfaceDescriptor();
	Testcases::texpainterFilterGraphImageSourceCall();
	Testcases::texpainterFilterGraphImageSourceName();
	return 0;
}