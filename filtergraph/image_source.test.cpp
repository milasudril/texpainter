//@	{
//@	 "targets":[{"name":"image_source.test", "type":"application", "autorun":1}]
//@	}

#include "./image_source.hpp"

#include "./image_processor.hpp"

#include <cassert>

namespace
{
	static_assert(Texpainter::FilterGraph::ImageProcessor2<
	              Texpainter::FilterGraph::ImageSource<Texpainter::FilterGraph::RgbaValue>>);
}

namespace Testcases
{
	void texpainterFilterGraphImageSourceInterfaceDescriptor()
	{
		static_assert(
		    Texpainter::FilterGraph::ImageSource<
		        Texpainter::FilterGraph::RealValue>::InterfaceDescriptor::InputPorts.size()
		    == 0);
		static_assert(
		    Texpainter::FilterGraph::ImageSource<
		        Texpainter::FilterGraph::RealValue>::InterfaceDescriptor::OutputPorts.size()
		    >= 1);

		static_assert(Texpainter::FilterGraph::ImageSource<
		                  Texpainter::FilterGraph::RealValue>::InterfaceDescriptor::OutputPorts[0]
		                  .type
		              == Texpainter::FilterGraph::PixelType::GrayscaleReal);
		static_assert(Texpainter::FilterGraph::ImageSource<
		                  Texpainter::FilterGraph::RgbaValue>::InterfaceDescriptor::OutputPorts[0]
		                  .type
		              == Texpainter::FilterGraph::PixelType::RGBA);
		static_assert(
		    Texpainter::FilterGraph::ImageSource<
		        Texpainter::FilterGraph::ComplexValue>::InterfaceDescriptor::OutputPorts[0]
		        .type
		    == Texpainter::FilterGraph::PixelType::GrayscaleComplex);
	}

	void texpainterFilterGraphImageSourceCall()
	{
		std::array<Texpainter::FilterGraph::RealValue, 6> pixels{1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
		auto size =Texpainter::Size2d{3, 2};
		Texpainter::FilterGraph::ImageSource<Texpainter::FilterGraph::RealValue> src{Texpainter::Span2d{pixels.data(), size}};
		using InterfaceDescriptor = Texpainter::FilterGraph::ImageSource<Texpainter::FilterGraph::RealValue>::InterfaceDescriptor;
		using ImgProcArg = Texpainter::FilterGraph::ImgProcArg2<InterfaceDescriptor>;
		using InputArgs = ImgProcArg::InputArgs;
		using OutputArgs = ImgProcArg::OutputArgs;

		std::array<Texpainter::FilterGraph::RealValue, 6> pixels_out{};
		OutputArgs args{};
		args.template get<0>() = pixels_out.data();
		src(ImgProcArg{size, InputArgs{}, args});
		assert(std::ranges::equal(pixels, pixels_out));
	}
}

#if 0


		void operator()(ImgProcArg2<InterfaceDescriptor> const& args) const
		{
			std::ranges::copy(r_pixels, args.template output<0>());
		}

		ParamValue get(ParamName) const { return ParamValue{0.0}; }

		void set(ParamName, ParamValue) {}


		explicit ImageSource(Span2d<PixelType const> pixels): r_pixels{pixels} {}

		static constexpr std::span<char const* const> paramNames()
		{
			return std::span<char const* const>{};
		}

		std::span<ParamValue const> paramValues() const { return std::span<ParamValue const>{}; }

		void set(std::string_view, ParamValue) {}

		ParamValue get(std::string_view) const { return ParamValue{0.0}; }

		static constexpr std::span<PortInfo const> outputPorts()
		{
			return std::span<PortInfo const>{s_output_ports};
		}

		static constexpr std::span<PortInfo const> inputPorts()
		{
			return std::span<PortInfo const>{};
		}

		std::vector<ImgProcRetval> operator()(std::span<ImgProcArg const>) const
		{
			return std::vector<ImgProcRetval>{PixelStore::BasicImage<PixelType>{r_pixels}};
		}


		static constexpr char const* name() { return "Layer input"; }

#endif

int main()
{
	Testcases::texpainterFilterGraphImageSourceInterfaceDescriptor();
	Testcases::texpainterFilterGraphImageSourceCall();
	return 0;
}