//@	{
//@	 "targets":[{"name":"rgba_to_grayalpha.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_IMGPROC_RGBATOGRAYALPHA_RGBATOGRAYALPHA_HPP
#define TEXPAINTER_IMGPROC_RGBATOGRAYALPHA_RGBATOGRAYALPHA_HPP

#include "filtergraph/port_info.hpp"
#include "filtergraph/img_proc_arg.hpp"
#include "filtergraph/image_processor_id.hpp"
#include "filtergraph/param_map.hpp"

namespace RgbaToGrayAlpha
{
	using Texpainter::vec4_t;
	using Texpainter::FilterGraph::ImageProcessorId;
	using Texpainter::FilterGraph::ImgProcArg;
	using Texpainter::FilterGraph::ParamMap;
	using Texpainter::FilterGraph::ParamName;
	using Texpainter::FilterGraph::ParamValue;
	using Texpainter::FilterGraph::PortInfo;
	using Texpainter::FilterGraph::PortType;
	using Texpainter::FilterGraph::RealValue;
	using Texpainter::FilterGraph::RgbaValue;

	vec4_t computeWeights(std::array<ParamValue, 3> const& vals)
	{
		std::array<double, 3> weights;
		// Reverse order because vals is sorted by param name, and Blue is before Red
		std::ranges::transform(vals, std::rbegin(weights), [](auto val) { return val.value(); });
		auto const sum = std::accumulate(std::begin(weights), std::end(weights), 0.0);
		if(sum == 0.0) { return vec4_t{1.0f, 1.0f, 1.0f, 0.0f} / 3.0f; }
		return vec4_t{static_cast<float>(weights[0]),
		              static_cast<float>(weights[1]),
		              static_cast<float>(weights[2]),
		              0.0f}
		       / static_cast<float>(sum);
	}

	class ImageProcessor
	{
	public:
		struct InterfaceDescriptor
		{
			static constexpr std::array<PortInfo, 1> InputPorts{{PortType::RgbaPixels, "Pixels"}};

			static constexpr std::array<PortInfo, 2> OutputPorts{
			    {{PortType::GrayscaleRealPixels, "Intensity"},
			     {PortType::GrayscaleRealPixels, "Alpha"}}};

			static constexpr std::array<ParamName, 3> ParamNames{"Red", "Green", "Blue"};
		};

		void operator()(ImgProcArg<InterfaceDescriptor> const& args) const
		{
			auto const weights = computeWeights(m_params.values());

			std::transform(args.input<0>(),
			               args.input<0>() + args.size().area(),
			               args.output<0>(),
			               [weights](auto val) {
				               auto weighted_val = val.value() * weights;
				               return std::clamp(weighted_val[0] + weighted_val[1] + weighted_val[2]
				                                     + weighted_val[3],
				                                 0.0f,
				                                 1.0f);
			               });

			std::transform(args.input<0>(),
			               args.input<0>() + args.size().area(),
			               args.output<1>(),
			               [](auto val) { return val.alpha(); });
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

		static constexpr char const* name() { return "Rgba to gray-alpha"; }

		static constexpr auto id() { return ImageProcessorId{"e87d6316bbcbde15f0543d66c79b9c75"}; }

	private:
		ParamMap<InterfaceDescriptor> m_params;
	};
}

#endif
