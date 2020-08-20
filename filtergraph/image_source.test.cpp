//@	{
//@	 "targets":[{"name":"image_source.test", "type":"application", "autorun":1}]
//@	}

#include "./image_source.hpp"

#include "./image_processor.hpp"

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
}

#if 0
		struct InterfaceDescriptor
		{
			static constexpr std::array<PortInfo, 0> InputPorts{};
			static constexpr std::array<PortInfo, 1> OutputPorts{
			    {{typeToPixelType<PixelType>(), "Pixels"}}};
			static constexpr std::array<ParamName, 0> ParamNames{};
		};

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
	return 0;
}