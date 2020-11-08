//@	{
//@	 "targets":[{"name":"map_grayscale_range.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"map_grayscale_range.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_IMGPROC_MAPGRAYSCALERANGE_MAPGRAYSCALERANGE_HPP
#define TEXPAINTER_IMGPROC_MAPGRAYSCALERANGE_MAPGRAYSCALERANGE_HPP

#include "filtergraph/port_info.hpp"
#include "filtergraph/img_proc_arg.hpp"
#include "filtergraph/image_processor_id.hpp"
#include "filtergraph/param_map.hpp"

namespace MapGrayscaleRange
{
	using Texpainter::Size2d;
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
			static constexpr std::array<PortInfo, 3> InputPorts{
			    {PortInfo{PortType::GrayscaleRealPixels, "Input"},
			     PortInfo{PortType::RealValue, "Min"},
			     PortInfo{PortType::RealValue, "Max"}}};

			static constexpr std::array<PortInfo, 1> OutputPorts{
			    PortInfo{PortType::GrayscaleRealPixels, "Output"}};

			static constexpr std::array<ParamName, 2> ParamNames{"Min", "Max"};
		};

		ImageProcessor()
		{
			*m_params.find<Str{"Min"}>() = ParamValue{0.0};
			*m_params.find<Str{"Max"}>() = ParamValue{1.0};
		}

		void operator()(ImgProcArg<InterfaceDescriptor> const& args) const;

		void set(ParamName name, ParamValue val)
		{
			if(auto ptr = m_params.find(name); ptr != nullptr) [[likely]]
				{
					*ptr = val;
				}
		}

		ParamValue get(ParamName name) const
		{
			auto ptr = m_params.find(name);
			return ptr != nullptr ? *ptr : ParamValue{0.0};
		}

		std::span<ParamValue const> paramValues() const { return m_params.values(); }

		static constexpr char const* name() { return "Map grayscale range"; }

		static constexpr auto id() { return ImageProcessorId{"3ac11258778bab13c5afe2ae91f9751e"}; }

	private:
		ParamMap<InterfaceDescriptor> m_params;
	};
}

#endif
