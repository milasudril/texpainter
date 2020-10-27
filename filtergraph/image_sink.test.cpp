//@	{
//@	 "targets":[{"name":"image_sink.test", "type":"application", "autorun":1}]
//@	}

#include "./image_sink.hpp"

#include "./image_processor.hpp"

#include <cassert>

namespace
{
	static_assert(Texpainter::FilterGraph::ImageProcessor<Texpainter::FilterGraph::ImageSink>);
}
namespace Testcases
{
	void texpainterFilterGraphImageSinkInterfaceDescriptor()
	{
		static_assert(Texpainter::FilterGraph::ImageSink::InterfaceDescriptor::InputPorts.size()
		              == 1);
		static_assert(Texpainter::FilterGraph::ImageSink::InterfaceDescriptor::OutputPorts.size()
		              == 0);
		static_assert(Texpainter::FilterGraph::ImageSink::InterfaceDescriptor::InputPorts[0].type
		              == Texpainter::FilterGraph::PortType::RgbaPixels);
	}

	void texpainterFilterGraphImageSinkCall()
	{
		auto size = Texpainter::Size2d{3, 2};
		Texpainter::FilterGraph::ImageSink sink;
		using InterfaceDescriptor = Texpainter::FilterGraph::ImageSink::InterfaceDescriptor;
		using ImgProcArg          = Texpainter::FilterGraph::ImgProcArg<InterfaceDescriptor>;
		using InputArgs           = ImgProcArg::InputArgs;
		using OutputArgs          = ImgProcArg::OutputArgs;
		using OutputBuffers =
		    Texpainter::FilterGraph::OutputBuffers<portTypes(InterfaceDescriptor::OutputPorts)>;

		std::array<Texpainter::FilterGraph::RgbaValue, 6> pixels{
		    Texpainter::FilterGraph::RgbaValue{1.0f, 0.0f, 0.0f, 1.0f},
		    Texpainter::FilterGraph::RgbaValue{1.0f, 1.0f, 0.0f, 1.0f},
		    Texpainter::FilterGraph::RgbaValue{0.0f, 1.0f, 0.0f, 1.0f},
		    Texpainter::FilterGraph::RgbaValue{0.0f, 1.0f, 1.0f, 1.0f},
		    Texpainter::FilterGraph::RgbaValue{0.0f, 0.0f, 1.0, 1.0f},
		    Texpainter::FilterGraph::RgbaValue{1.0f, 0.0f, 1.0, 1.0f}};

		InputArgs args{Texpainter::FilterGraph::NodeArgument{size, {{pixels.data()}}}};

		std::array<Texpainter::FilterGraph::RgbaValue, 6> pixels_out{};
		sink.sink(Texpainter::Span2d{pixels_out.data(), size});

		OutputBuffers buffers{size};

		sink(ImgProcArg{size, args, OutputArgs{buffers}});
		assert(std::ranges::equal(pixels, pixels_out, [](auto a, auto b) {
			auto diff = a - b;
			return diff.red() == 0.0f && diff.green() == 0.0f && diff.blue() == 0.0f
			       && diff.alpha() == 0.0f;
		}));
	}

	void texpainterFilterGraphImageSinkName()
	{
		static_assert(Texpainter::FilterGraph::ImageSink::name() != nullptr);
		static_assert(*Texpainter::FilterGraph::ImageSink::name() != '\0');
	}
}

int main()
{
	Testcases::texpainterFilterGraphImageSinkInterfaceDescriptor();
	Testcases::texpainterFilterGraphImageSinkCall();
	Testcases::texpainterFilterGraphImageSinkName();
	return 0;
}