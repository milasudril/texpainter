//@	{
//@	 "targets":[{"name":"butterworth_mask_1d.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"butterworth_mask_1d.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_IMGPROC_BUTTERWORTHMASK1D_BUTTERWORTHMASK1D_HPP
#define TEXPAINTER_IMGPROC_BUTTERWORTHMASK1D_BUTTERWORTHMASK1D_HPP

#include "filtergraph/proctypes.hpp"
#include "filtergraph/img_proc_arg.hpp"
#include "filtergraph/image_processor_id.hpp"
#include "filtergraph/param_map.hpp"

namespace ButterworthMask1d
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
			    {PortInfo{PortType::GrayscaleReal, "Output"}}};

			static constexpr std::array<ParamName, 3> ParamNames{"Radius", "Order", "Orientation"};
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

		static constexpr char const* name() { return "Butterworth mask 1d"; }

		static constexpr auto id() { return ImageProcessorId{"744774f5bdf17e218773b07d5f9ef5b7"}; }

	private:
		ParamMap<InterfaceDescriptor> m_params;
	};
}

#endif
