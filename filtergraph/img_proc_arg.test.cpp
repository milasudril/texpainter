//@	{
//@	 "targets":[{"name":"img_proc_arg.test", "type":"application", "autorun":1}]
//@	}

#include "./img_proc_arg.hpp"

#include <cassert>

namespace
{
	struct ImgProcStub
	{
		static constexpr auto inputPorts()
		{
			return std::array<Texpainter::FilterGraph::PortInfo, 3>{
			    {{Texpainter::FilterGraph::PixelType::RGBA, "Input 1"},
			     {Texpainter::FilterGraph::PixelType::GrayscaleReal, "Input 2"},
			     {Texpainter::FilterGraph::PixelType::GrayscaleComplex, "Input 3"}}};
		}

		static constexpr auto outputPorts()
		{
			return std::array<Texpainter::FilterGraph::PortInfo, 3>{
			    {{Texpainter::FilterGraph::PixelType::GrayscaleComplex, "Output 1"},
			     {Texpainter::FilterGraph::PixelType::GrayscaleReal, "Output 2"},
			     {Texpainter::FilterGraph::PixelType::RGBA, "Output 3"}}};
		}

		static constexpr char const* name() { return "Stub"; }
		static constexpr auto paramNames() { return std::span<char const* const>{}; }
		void set(std::string_view, Texpainter::FilterGraph::ParamValue) {}
		auto get(std::string_view) const { return Texpainter::FilterGraph::ParamValue{0.0f}; }
		auto paramValues() const { return std::span<Texpainter::FilterGraph::ParamValue const>{}; }

		auto operator()(std::span<Texpainter::FilterGraph::ImgProcArg const>) const
		{
			return std::vector<Texpainter::FilterGraph::ImgProcRetval>{};
		}
	};
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

		std::array<Texpainter::PixelStore::Pixel, size.area()> output1{};
		std::array<Texpainter::FilterGraph::RealValue, size.area()> output2{};
		std::array<Texpainter::FilterGraph::ComplexValue, size.area()> output3{};

		Texpainter::FilterGraph::NodeArgument na{size, {std::begin(input1), std::begin(input2), std::begin(input3)}
			,{std::begin(output1), std::begin(output2), std::begin(output3)}
		};
		Texpainter::FilterGraph::ImgProcArg2<ImgProcStub> obj{na};

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
	}
}

int main() { Testcases::texpainterFilterGraphImgProcArgTest(); }