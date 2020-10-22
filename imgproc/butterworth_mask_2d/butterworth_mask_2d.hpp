//@	{
//@	 "targets":[{"name":"butterworth_mask_2d.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"butterworth_mask_2d.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_IMGPROC_BUTTERWORTHMASK2D_BUTTERWORTHMASK2D_HPP
#define TEXPAINTER_IMGPROC_BUTTERWORTHMASK2D_BUTTERWORTHMASK2D_HPP

#include "filtergraph/proctypes.hpp"
#include "filtergraph/img_proc_arg.hpp"
#include "filtergraph/image_processor_id.hpp"
#include "filtergraph/param_map.hpp"

namespace ButterworthMask2d
{
	using Texpainter::Size2d;
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
			static constexpr std::array<PortInfo, 0> InputPorts{};
			static constexpr std::array<PortInfo, 1> OutputPorts{
			    {PortInfo{PortType::GrayscaleRealPixels, "Output"}}};

			static constexpr std::array<ParamName, 4> ParamNames{
			    "Semi-axis 1", "Semi-axis 2", "Order", "Orientation"};
		};

		void operator()(ImgProcArg<InterfaceDescriptor> const& args) const;

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

		static constexpr char const* name() { return "Butterworth mask 2d"; }

		static constexpr auto id() { return ImageProcessorId{"95dda30a7b662de940ad80180d0b2943"}; }

	private:
		ParamMap<InterfaceDescriptor> m_params;
	};
}

#endif
