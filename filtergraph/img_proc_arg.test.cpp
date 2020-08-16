//@	{
//@	 "targets":[{"name":"img_proc_arg.test", "type":"application", "autorun":1}]
//@	}

#include "./img_proc_arg.hpp"

#include <cassert>

namespace
{
	struct Ports
	{
		struct InterfaceDescriptor
		{
			static constexpr auto InputPorts = std::array<Texpainter::FilterGraph::PortInfo, 3>{
			    {{Texpainter::FilterGraph::PixelType::RGBA, "Input 1"},
			     {Texpainter::FilterGraph::PixelType::GrayscaleReal, "Input 2"},
			     {Texpainter::FilterGraph::PixelType::GrayscaleComplex, "Input 3"}}};

			static constexpr auto OutputPorts = std::array<Texpainter::FilterGraph::PortInfo, 3>{
			    {{Texpainter::FilterGraph::PixelType::GrayscaleComplex, "Output 1"},
			     {Texpainter::FilterGraph::PixelType::GrayscaleReal, "Output 2"},
			     {Texpainter::FilterGraph::PixelType::RGBA, "Output 3"}}};
		};
	};

	using InArgs =
	    Texpainter::FilterGraph::InArgTuple<portTypes(Ports::InterfaceDescriptor::InputPorts)>;
	using OutArgs =
	    Texpainter::FilterGraph::OutArgTuple<portTypes(Ports::InterfaceDescriptor::OutputPorts)>;
}

namespace Testcases
{
	void texpainterFilterGraphImgProcArgTest()
	{
		constexpr Texpainter::Size2d size{3, 2};
		std::array<Texpainter::PixelStore::Pixel, size.area()> const input1{};
		std::array<Texpainter::FilterGraph::RealValue, size.area()> const input2{
		    1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
		std::array<Texpainter::FilterGraph::ComplexValue, size.area()> const input3{};

		std::array<Texpainter::FilterGraph::ComplexValue, size.area()> output1{};
		std::array<Texpainter::FilterGraph::RealValue, size.area()> output2{};
		std::array<Texpainter::PixelStore::Pixel, size.area()> output3{};


		Texpainter::FilterGraph::ImgProcArg2<Ports> const obj{
		    size,
		    InArgs{std::begin(input3), std::begin(input2), std::begin(input1)},
		    OutArgs{std::begin(output3), std::begin(output2), std::begin(output1)}};

		assert(obj.size() == size);

		assert(obj.input<1>(0, 0) == input2[0]);
		assert(obj.input<1>(2, 0) == input2[2]);
		assert(obj.input<1>(0, 1) == input2[3]);
		assert(obj.input<1>(2, 1) == input2[5]);

		static_assert(std::is_same_v<decltype(obj.input<0>(0, 0)), Texpainter::PixelStore::Pixel>);
		static_assert(
		    std::is_same_v<decltype(obj.input<1>(0, 0)), Texpainter::FilterGraph::RealValue>);
		static_assert(
		    std::is_same_v<decltype(obj.input<2>(0, 0)), Texpainter::FilterGraph::ComplexValue>);

		static_assert(
		    std::is_same_v<decltype(obj.input<0>()), Texpainter::PixelStore::Pixel const*>);
		static_assert(
		    std::is_same_v<decltype(obj.input<1>()), Texpainter::FilterGraph::RealValue const*>);
		static_assert(
		    std::is_same_v<decltype(obj.input<2>()), Texpainter::FilterGraph::ComplexValue const*>);

		auto ptr = obj.output<0>();
		*ptr     = Texpainter::FilterGraph::ComplexValue{1, 2};
		static_assert(std::is_same_v<decltype(ptr), Texpainter::FilterGraph::ComplexValue*>);
		assert((*ptr == Texpainter::FilterGraph::ComplexValue{1, 2}));
		assert(ptr == std::begin(output1));
	}
}

int main() { Testcases::texpainterFilterGraphImgProcArgTest(); }