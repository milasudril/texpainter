//@	{
//@	 "targets":[{"name":"imageprocessorwrapper.test", "type":"application", "autorun":1}]
//@	}

#include "./image_processor_wrapper.hpp"

#include <cassert>

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

	void operator()(auto const& args) const
	{
		*called_with_size = args.size();
	}

	Texpainter::Size2d* called_with_size;
};

namespace Testcases
{
	void texpaitnerFilterGraphImageProcessorWrapperCall()
	{
		Texpainter::Size2d size_out{1, 1};
		Texpainter::FilterGraph::ImageProcessorWrapper obj{ImgProcStub{&size_out}};

		Texpainter::Size2d size{3, 2};
		Texpainter::FilterGraph::NodeArgument na{size, {{nullptr, nullptr, nullptr}}};
		auto ret = obj(na);
		assert(size_out == size);
	}
}

int main()
{
	Testcases::texpaitnerFilterGraphImageProcessorWrapperCall();
	return 0;
}