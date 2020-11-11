//@	{
//@	 "targets":[{"name":"project_normal.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_IMGPROC_PROJECTNORMAL_PROJECTNORMAL_HPP
#define TEXPAINTER_IMGPROC_PROJECTNORMAL_PROJECTNORMAL_HPP

#include "filtergraph/port_info.hpp"
#include "filtergraph/img_proc_arg.hpp"
#include "filtergraph/image_processor_id.hpp"
#include "filtergraph/param_map.hpp"

#include <numbers>
#include <cmath>

namespace ProjectNormal
{
	using Texpainter::Str;
	using Texpainter::vec4_t;
	using Texpainter::FilterGraph::ImageProcessorId;
	using Texpainter::FilterGraph::ImgProcArg;
	using Texpainter::FilterGraph::ParamMap;
	using Texpainter::FilterGraph::ParamName;
	using Texpainter::FilterGraph::ParamValue;
	using Texpainter::FilterGraph::PortInfo;
	using Texpainter::FilterGraph::PortType;
	using Texpainter::FilterGraph::TopographyInfo;

	class ImageProcessor
	{
	public:
		struct InterfaceDescriptor
		{
			static constexpr std::array<PortInfo, 1> OutputPorts{
			    {PortType::GrayscaleRealPixels, "Pixels"}};

			static constexpr std::array<PortInfo, 1> InputPorts{
			    {{PortType::TopographyData, "Topography data"}}};

			static constexpr std::array<ParamName, 2> ParamNames{"Azimuth angle", "Zenith angle"};
		};

		void operator()(ImgProcArg<InterfaceDescriptor> const& arg) const
		{
			auto const phi =
			    2.0 * std::numbers::pi * m_params.find<Str{"Azimuth angle"}>()->value();
			auto const theta =
			    0.5 * std::numbers::pi * m_params.find<Str{"Zenith angle"}>()->value();
			vec4_t proj{static_cast<float>(sin(theta) * cos(phi)),
			            static_cast<float>(sin(theta) * sin(phi)),
			            static_cast<float>(cos(theta)),
			            0.0f};

			std::transform(arg.input<0>(),
			               arg.input<0>() + arg.size().area(),
			               arg.output<0>(),
			               [proj](auto val) { return Texpainter::dot(val.value(), proj); });
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

		static constexpr char const* name() { return "Project normal"; }

		static constexpr auto id() { return ImageProcessorId{"dd205464d683126a9da4972613e849e4"}; }

	private:
		ParamMap<InterfaceDescriptor> m_params;
	};
}

#endif
