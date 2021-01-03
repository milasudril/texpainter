//@	{
//@	 "targets":[{"name":"img_proc_arg.test", "type":"application", "autorun":1}]
//@	}

#include "./img_proc_arg.hpp"

#include <cassert>

namespace
{
	struct Ports
	{
		static constexpr auto InputPorts = std::array<Texpainter::FilterGraph::PortInfo, 3>{
		    {{Texpainter::FilterGraph::PortType::RgbaPixels, "Input 1"},
		     {Texpainter::FilterGraph::PortType::GrayscaleRealPixels, "Input 2"},
		     {Texpainter::FilterGraph::PortType::GrayscaleComplexPixels, "Input 3"}}};

		static constexpr auto OutputPorts = std::array<Texpainter::FilterGraph::PortInfo, 3>{
		    {{Texpainter::FilterGraph::PortType::GrayscaleComplexPixels, "Output 1"},
		     {Texpainter::FilterGraph::PortType::GrayscaleRealPixels, "Output 2"},
		     {Texpainter::FilterGraph::PortType::RgbaPixels, "Output 3"}}};

		static constexpr auto ParamNames =
		    std::array<Texpainter::FilterGraph::ParamName, 1>{nullptr};
	};

	using InArgs  = Texpainter::FilterGraph::InArgTuple<portTypes(Ports::InputPorts)>;
	using OutArgs = Texpainter::FilterGraph::OutArgTuple<portTypes(Ports::OutputPorts)>;
}

namespace Testcases
{
	void texpainterFilterGraphImgProcArgTest()
	{
		constexpr Texpainter::Size2d size{3, 2};
		std::array<Texpainter::PixelStore::Pixel, area(size)> const input1{};
		std::array<Texpainter::FilterGraph::RealValue, area(size)> const input2{
		    1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
		std::array<Texpainter::FilterGraph::ComplexValue, area(size)> const input3{};

		std::array<Texpainter::FilterGraph::ComplexValue, area(size)> output1{};
		std::array<Texpainter::FilterGraph::RealValue, area(size)> output2{};
		std::array<Texpainter::PixelStore::Pixel, area(size)> output3{};

		Texpainter::FilterGraph::ImgProcArg<Ports> const obj{
		    size,
		    2.0,
		    InArgs{std::begin(input1), std::begin(input2), std::begin(input3)},
		    OutArgs{std::begin(output1), std::begin(output2), std::begin(output3)}};

		assert(obj.canvasSize() == size);
		assert(obj.resolution() == 2.0);

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