//@	{
//@	 "targets":[{"name":"mix_complex.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_IMGPROC_MIXCOMPLEX_MIXCOMPLEX_HPP
#define TEXPAINTER_IMGPROC_MIXCOMPLEX_MIXCOMPLEX_HPP

#include "filtergraph/port_info.hpp"
#include "filtergraph/img_proc_arg.hpp"
#include "filtergraph/image_processor_id.hpp"

#include <algorithm>

namespace MixComplex
{
	using Texpainter::Str;
	using Texpainter::FilterGraph::ComplexValue;
	using Texpainter::FilterGraph::ImageProcessorId;
	using Texpainter::FilterGraph::ImgProcArg;
	using Texpainter::FilterGraph::ParamMap;
	using Texpainter::FilterGraph::ParamName;
	using Texpainter::FilterGraph::ParamValue;
	using Texpainter::FilterGraph::PortInfo;
	using Texpainter::FilterGraph::PortType;

	class ImageProcessor
	{
	public:
		struct InterfaceDescriptor
		{
			static constexpr std::array<PortInfo, 2> InputPorts{
			    {PortInfo{PortType::GrayscaleComplexPixels, "Input A"},
			     PortInfo{PortType::GrayscaleComplexPixels, "Input B"}}};
			static constexpr std::array<PortInfo, 1> OutputPorts{
			    {PortType::GrayscaleComplexPixels, "Output"}};

			static constexpr std::array<ParamName, 1> ParamNames{"Blend factor"};
		};

		void operator()(ImgProcArg<InterfaceDescriptor> const& args) const
		{
			auto const size = args.size().area();
			std::transform(args.input<0>(),
			               args.input<0>() + size,
			               args.input<1>(),
			               args.output<0>(),
			               [t = *m_params.find<Str{"Blend factor"}>()](auto a, auto b) {
				               return a + t.value() * (b - a);
			               });
		}

		void set(ParamName name, ParamValue value)
		{
			if(auto ptr = m_params.find(name); ptr != nullptr) [[likely]]
				{
					*ptr = value;
				}
		}

		ParamValue get(ParamName name) const
		{
			auto ptr = m_params.find(name);
			return ptr != nullptr ? *ptr : ParamValue{0.0};
		}

		std::span<ParamValue const> paramValues() const { return m_params.values(); }

		static constexpr char const* name() { return "Mix spectra"; }

		static constexpr auto id() { return ImageProcessorId{"1845d33d870ee84be36344fdb450e09f"}; }

	private:
		ParamMap<InterfaceDescriptor> m_params;
	};
}

#endif
