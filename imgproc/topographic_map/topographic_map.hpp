//@	{
//@	 "targets":[{"name":"topographic_map.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_IMGPROC_TOPOGRAPHIC_MAP_HPP
#define TEXPAINTER_IMGPROC_TOPOGRAPHIC_MAP_HPP

#include "filtergraph/port_info.hpp"
#include "filtergraph/img_proc_arg.hpp"
#include "filtergraph/image_processor_id.hpp"
#include "filtergraph/param_map.hpp"

#include <cmath>

namespace TopographicMap
{
	using Texpainter::Str;
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
			static constexpr std::array<PortInfo, 1> InputPorts{
			    {PortType::GrayscaleRealPixels, "Pixels"}};

			static constexpr std::array<PortInfo, 1> OutputPorts{
			    {{PortType::TopographyData, "Topography data"}}};

			static constexpr std::array<ParamName, 1> ParamNames{"Max elevation"};
		};

		void operator()(ImgProcArg<InterfaceDescriptor> const& arg) const
		{
			auto size = arg.size();
			auto height =
			    sqrt(size.area())
			    * exp2(std::lerp(-4.0, 4.0, m_params.find<Str{"Max elevation"}>()->value()));
			for(uint32_t row = 0; row < size.height(); ++row)
			{
				for(uint32_t col = 0; col < size.width(); ++col)
				{
					auto const dx = arg.input<0>((col + 1 + size.width()) % size.width(), row)
					                - arg.input<0>((col - 1 + size.width()) % size.width(), row);
					auto const dy = arg.input<0>(col, (row + 1 + size.height()) % size.height())
					                - arg.input<0>(col, (row - 1 + size.height()) % size.height());

					arg.output<0>(col, row) =
					    TopographyInfo{static_cast<float>(height * 0.5 * dx),
					                   static_cast<float>(height * 0.5 * dy),
					                   static_cast<float>(height * arg.input<0>(col, row))};
				}
			}
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

		static constexpr char const* name() { return "Topographic map"; }

		static constexpr auto id() { return ImageProcessorId{"177ece2ccf47903b91eacaf01b0ebc51"}; }

	private:
		ParamMap<InterfaceDescriptor> m_params;
	};
}

#endif
