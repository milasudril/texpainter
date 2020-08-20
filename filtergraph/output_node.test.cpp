//@	{
//@	 "targets":[{"name":"output_node.test", "type":"application", "autorun":1}]
//@	}

#include "./output_node.hpp"

#include "./image_processor.hpp"

#include <cassert>

namespace
{
	static_assert(Texpainter::FilterGraph::ImageProcessor2<Texpainter::FilterGraph::OutputNode>);
}
namespace Testcases
{
	void texpainterFilterGraphOutputNodeInterfaceDescriptor()
	{
		static_assert(Texpainter::FilterGraph::OutputNode::InterfaceDescriptor::InputPorts.size()
		              == 1);
		static_assert(Texpainter::FilterGraph::OutputNode::InterfaceDescriptor::OutputPorts.size()
		              == 0);
		static_assert(Texpainter::FilterGraph::OutputNode::InterfaceDescriptor::InputPorts[0].type
		              == Texpainter::FilterGraph::PixelType::RGBA);
	}

	void texpainterFilterGraphOutputNodeCall()
	{
		auto size = Texpainter::Size2d{3, 2};
		Texpainter::FilterGraph::OutputNode sink;
		using InterfaceDescriptor = Texpainter::FilterGraph::OutputNode::InterfaceDescriptor;
		using ImgProcArg          = Texpainter::FilterGraph::ImgProcArg2<InterfaceDescriptor>;
		using InputArgs           = ImgProcArg::InputArgs;
		using OutputArgs          = ImgProcArg::OutputArgs;

		InputArgs args{};
		std::array<Texpainter::FilterGraph::RgbaValue, 6> pixels{
		    Texpainter::FilterGraph::RgbaValue{1.0f, 0.0f, 0.0f, 1.0f},
		    Texpainter::FilterGraph::RgbaValue{1.0f, 1.0f, 0.0f, 1.0f},
		    Texpainter::FilterGraph::RgbaValue{0.0f, 1.0f, 0.0f, 1.0f},
		    Texpainter::FilterGraph::RgbaValue{0.0f, 1.0f, 1.0f, 1.0f},
		    Texpainter::FilterGraph::RgbaValue{0.0f, 0.0f, 1.0, 1.0f},
		    Texpainter::FilterGraph::RgbaValue{1.0f, 0.0f, 1.0, 1.0f}};
		args.template get<0>() = pixels.data();

		std::array<Texpainter::FilterGraph::RgbaValue, 6> pixels_out{};
		sink.pixels(Texpainter::Span2d{pixels_out.data(), size});

		sink(ImgProcArg{size, args, OutputArgs{}});
		assert(std::ranges::equal(pixels, pixels_out, [](auto a, auto b) {
			auto diff = a - b;
			return diff.red() == 0.0f && diff.green() == 0.0f && diff.blue() == 0.0f
			       && diff.alpha() == 0.0f;
		}));
	}

	void texpainterFilterGraphOutputNodeName()
	{
		static_assert(Texpainter::FilterGraph::OutputNode::name() != nullptr);
		static_assert(*Texpainter::FilterGraph::OutputNode::name() != '\0');
	}
}

int main()
{
	Testcases::texpainterFilterGraphOutputNodeInterfaceDescriptor();
	Testcases::texpainterFilterGraphOutputNodeCall();
	Testcases::texpainterFilterGraphOutputNodeName();
	return 0;
}