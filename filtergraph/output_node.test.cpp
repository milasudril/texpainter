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

#if 0
	void texpainterFilterGraphOutputNodeCall()
	{
		std::array<Texpainter::FilterGraph::RealValue, 6> pixels{1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
		auto size = Texpainter::Size2d{3, 2};
		Texpainter::FilterGraph::OutputNode<Texpainter::FilterGraph::RealValue> src{
		    Texpainter::Span2d{pixels.data(), size}};
		using InterfaceDescriptor = Texpainter::FilterGraph::OutputNode<
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

	void texpainterFilterGraphOutputNodeName()
	{
		static_assert(Texpainter::FilterGraph::OutputNode::name() != nullptr);
		static_assert(*Texpainter::FilterGraph::OutputNode::name() != '\0');
	}
}

int main()
{
	Testcases::texpainterFilterGraphOutputNodeInterfaceDescriptor();
#if 0
	Testcases::texpainterFilterGraphOutputNodeCall();
#endif
	Testcases::texpainterFilterGraphOutputNodeName();
	return 0;
}