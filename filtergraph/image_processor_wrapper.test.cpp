//@	{
//@	 "targets":[{"name":"image_processor_wrapper.test", "type":"application", "autorun":1}]
//@	}

#include "./image_processor_wrapper.hpp"

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
		static constexpr auto paramNames()
		{
			return std::span<char const* const>{};
		}
		void set(std::string_view, Texpainter::FilterGraph::ParamValue) {}
		auto get(std::string_view) const { return Texpainter::FilterGraph::ParamValue{0.0f}; }
		auto paramValues() const { return std::span<Texpainter::FilterGraph::ParamValue const>{}; }

		auto operator()(std::span<Texpainter::FilterGraph::ImgProcArg const>) const
		{
			return std::vector<Texpainter::FilterGraph::ImgProcRetval>{};
		}

		void operator()(Texpainter::FilterGraph::ImgProcArg2<ImgProcStub> const& args) const
		{
			*args_result = args;
		}

		mutable Texpainter::FilterGraph::ImgProcArg2<ImgProcStub>* args_result;
	};
}

namespace Testcases
{
	void texpaitnerFilterGraphImageProcessorWrapperCall()
	{
		using ImgProcArg = Texpainter::FilterGraph::ImgProcArg2<ImgProcStub>;
		ImgProcArg args{
		    Texpainter::Size2d{1, 2}, ImgProcArg::InputArgs{}, ImgProcArg::OutputArgs{}};
		Texpainter::FilterGraph::ImageProcessorWrapper obj{ImgProcStub{&args}};

		Texpainter::Size2d size{3, 2};

		std::array<Texpainter::FilterGraph::RgbaValue, 6> input1{};
		std::array<Texpainter::FilterGraph::RealValue, 6> input2{};
		std::array<Texpainter::FilterGraph::ComplexValue, 6> input3{};


		Texpainter::FilterGraph::NodeArgument na{size, {{std::begin(input1), std::begin(input2), std::begin(input3)}}};
		auto ret = obj(na);
		assert(args.size() == size);
		assert(ret[0].get() == args.output<0>());
		assert(ret[1].get() == args.output<1>());
		assert(ret[2].get() == args.output<2>());
		static_assert(std::is_same_v<decltype(args.output<0>()), Texpainter::FilterGraph::ComplexValue*>);
		static_assert(std::is_same_v<decltype(args.output<1>()), Texpainter::FilterGraph::RealValue*>);
		static_assert(std::is_same_v<decltype(args.output<2>()), Texpainter::FilterGraph::RgbaValue*>);

		assert(std::begin(input1) == args.input<0>());
		assert(std::begin(input2) == args.input<1>());
		assert(std::begin(input3) == args.input<2>());
		static_assert(std::is_same_v<decltype(args.input<0>()), Texpainter::FilterGraph::RgbaValue const*>);
		static_assert(std::is_same_v<decltype(args.input<1>()), Texpainter::FilterGraph::RealValue const*>);
		static_assert(std::is_same_v<decltype(args.input<2>()), Texpainter::FilterGraph::ComplexValue const*>);
	}
}

int main()
{
	Testcases::texpaitnerFilterGraphImageProcessorWrapperCall();
	return 0;
}