//@	{
//@	 "targets":[{"name":"topographic_map.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_IMGPROC_TOPOGRAPHIC_MAP_HPP
#define TEXPAINTER_IMGPROC_TOPOGRAPHIC_MAP_HPP

#include "filtergraph/port_info.hpp"
#include "filtergraph/img_proc_arg.hpp"
#include "filtergraph/image_processor_id.hpp"
#include "filtergraph/param_map.hpp"

namespace TopographicMap
{
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

			static constexpr std::array<ParamName, 0> ParamNames{};
		};

		void operator()(ImgProcArg<InterfaceDescriptor> const& arg) const
		{
			auto size = arg.size();
			// FIXME: boundary conditions
			for(uint32_t row = 1; row < size.height() - 1; ++row)
			{
				for(uint32_t col = 1; col < size.width() - 1; ++col)
				{
					auto const dx = arg.input<0>(col + 1, row) - arg.input<0>(col - 1, row);
					auto const dy = arg.input<0>(col, row + 1) - arg.input<0>(col, row - 1);

					arg.output<0>(col, row) =
					    TopographyInfo{static_cast<float>(dx),
					                   static_cast<float>(dy),
					                   static_cast<float>(arg.input<0>(col, row))};
				}
			}
		}

		void set(ParamName, ParamValue) {}

		ParamValue get(ParamName) const { return ParamValue{0.0}; }

		std::span<ParamValue const> paramValues() const { return std::span<ParamValue const>{}; }

		static constexpr char const* name() { return "Topographic map"; }

		static constexpr auto id() { return ImageProcessorId{"177ece2ccf47903b91eacaf01b0ebc51"}; }

	private:
	};
}

#endif
