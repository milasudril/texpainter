//@	{
//@	 "targets":[{"name":"mix_grayscale.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_IMGPROC_MIXGRAYSCALE_MIXGRAYSCALE_HPP
#define TEXPAINTER_IMGPROC_MIXGRAYSCALE_MIXGRAYSCALE_HPP

#include "filtergraph/proctypes.hpp"
#include "filtergraph/img_proc_arg.hpp"
#include "filtergraph/image_processor_id.hpp"

#include <algorithm>
#include <cmath>

namespace MixGrayscale
{
	using Texpainter::Str;
	using Texpainter::FilterGraph::ImageProcessorId;
	using Texpainter::FilterGraph::ImgProcArg;
	using Texpainter::FilterGraph::ParamMap;
	using Texpainter::FilterGraph::ParamName;
	using Texpainter::FilterGraph::ParamValue;
	using Texpainter::FilterGraph::PortInfo;
	using Texpainter::FilterGraph::PortType;
	using Texpainter::FilterGraph::RealValue;

	class ImageProcessor
	{
	public:
		struct InterfaceDescriptor
		{
			static constexpr std::array<PortInfo, 2> InputPorts{
			    {PortInfo{PortType::GrayscaleRealPixels, "Input A"},
			     PortInfo{PortType::GrayscaleRealPixels, "Input B"}}};
			static constexpr std::array<PortInfo, 1> OutputPorts{
			    {PortType::GrayscaleRealPixels, "Output"}};

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
				               return std::lerp(a, b, t.value());
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

		static constexpr char const* name() { return "Mix grayscale"; }

		static constexpr auto id() { return ImageProcessorId{"27180c19b176cfa0e91c899b2a54c165"}; }

	private:
		ParamMap<InterfaceDescriptor> m_params;
	};
}

#endif
