//@	{
//@	 "targets":[{"name":"image_source.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_IMAGESOURCE_HPP
#define TEXPAINTER_FILTERGRAPH_IMAGESOURCE_HPP

#include "./port_info.hpp"
#include "./img_proc_arg.hpp"
#include "./image_processor_id.hpp"

#include "utils/span_2d.hpp"

namespace Texpainter::FilterGraph
{
	namespace detail
	{
		template<class T>
		struct PortTypeToImgSourceId;

		template<>
		struct PortTypeToImgSourceId<RgbaValue>
		{
			static constexpr auto id()
			{
				return ImageProcessorId{"477b3a43f2eea34f6b6f4e358a0ba919"};
			}
		};

		template<>
		struct PortTypeToImgSourceId<RealValue>
		{
			static constexpr auto id()
			{
				return ImageProcessorId{"64f8854706a66e638dca3b5abe986db6"};
			}
		};

		template<>
		struct PortTypeToImgSourceId<ComplexValue>
		{
			static constexpr auto id()
			{
				return ImageProcessorId{"fd422979a8130f2940f84570b5956c31"};
			}
		};


	}

	template<class PortType>
	class ImageSource
	{
	public:
		struct InterfaceDescriptor
		{
			static constexpr std::array<PortInfo, 0> InputPorts{};
			static constexpr std::array<PortInfo, 1> OutputPorts{
			    {{typeToPortType<PortType>(), "Pixels"}}};
			static constexpr std::array<ParamName, 0> ParamNames{};
		};

		void operator()(ImgProcArg<InterfaceDescriptor> const& args) const
		{
			std::ranges::copy(r_pixels, args.template output<0>());
		}

		ParamValue get(ParamName) const { return ParamValue{0.0}; }

		void set(ParamName, ParamValue) {}

		std::span<ParamValue const> paramValues() const { return std::span<ParamValue const>{}; }

		static constexpr char const* name() { return "Layer input"; }

		static constexpr auto id() { return detail::PortTypeToImgSourceId<PortType>::id(); }

		void source(Span2d<PortType const> src) { r_pixels = src; }

	private:
		Span2d<PortType const> r_pixels;
	};
}

#endif
