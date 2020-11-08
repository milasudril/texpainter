//@	{
//@	 "targets":[{"name":"soft_clamp.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_IMGPROC_SOFTCLAMP_HPP
#define TEXPAINTER_IMGPROC_SOFTCLAMP_HPP

#include "filtergraph/port_info.hpp"
#include "filtergraph/img_proc_arg.hpp"
#include "filtergraph/image_processor_id.hpp"
#include "filtergraph/param_map.hpp"

namespace SoftClamp
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
	using Texpainter::FilterGraph::RgbaValue;

	float computeValue(float x, float x_min, float x_max, float softness)
	{
		auto xi = (2 * x - x_min - x_max) / (x_max - x_min);
		return xi <= -1.0f ? 0.0f
		                   : (xi >= 1.0f ? 1.0f
		                                 : std::lerp(0.5f * (xi + 1.0f),
		                                             (xi * (3 - xi * xi) + 2.0f) / 4.0f,
		                                             softness));
	}

	class ImageProcessor
	{
	public:
		struct InterfaceDescriptor
		{
			static constexpr std::array<PortInfo, 1> InputPorts{
			    {PortType::GrayscaleRealPixels, "Input"}};

			static constexpr std::array<PortInfo, 1> OutputPorts{
			    {{PortType::GrayscaleRealPixels, "Output"}}};

			static constexpr std::array<ParamName, 3> ParamNames{"Min", "Max", "Softness"};
		};

		void operator()(ImgProcArg<InterfaceDescriptor> const& args) const
		{
			auto const x_min    = static_cast<float>(m_params.find<Str{"Min"}>()->value());
			auto const x_max    = static_cast<float>(m_params.find<Str{"Max"}>()->value());
			auto const softness = static_cast<float>(m_params.find<Str{"Softness"}>()->value());
			std::transform(args.input<0>(),
			               args.input<0>() + args.size().area(),
			               args.output<0>(),
			               [x_min, x_max, softness](auto val) {
				               return computeValue(val, x_min, x_max, softness);
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

		static constexpr char const* name() { return "Soft clamp"; }

		static constexpr auto id() { return ImageProcessorId{"2eccfc3419adbd043a49b10ac45cb383"}; }

	private:
		ParamMap<InterfaceDescriptor> m_params;
	};
}

#endif
