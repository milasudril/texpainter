//@	{
//@	 "targets":[{"name":"gaussian_mask.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"gaussian_mask.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_IMGPROC_GAUSSIANMASK_GAUSSIANMASK_HPP
#define TEXPAINTER_IMGPROC_GAUSSIANMASK_GAUSSIANMASK_HPP

#include "filtergraph/proctypes.hpp"
#include "filtergraph/img_proc_arg.hpp"
#include "filtergraph/image_processor_id.hpp"
#include "filtergraph/param_map.hpp"
#include "pixel_store/image.hpp"

namespace GaussianMask
{
	using Texpainter::Size2d;
	using Texpainter::FilterGraph::ImageProcessorId;
	using Texpainter::FilterGraph::ImgProcArg;
	using Texpainter::FilterGraph::ParamMap;
	using Texpainter::FilterGraph::ParamName;
	using Texpainter::FilterGraph::ParamValue;
	using Texpainter::FilterGraph::PixelType;
	using Texpainter::FilterGraph::PortInfo;
	using Texpainter::FilterGraph::RealValue;
	using Texpainter::PixelStore::BasicImage;

	class ImageProcessor
	{
	public:
		struct InterfaceDescriptor
		{
			static constexpr std::array<PortInfo, 0> InputPorts{};
			static constexpr std::array<PortInfo, 1> OutputPorts{
			    {PortInfo{PixelType::GrayscaleReal, "Output"}}};

			static constexpr std::array<ParamName, 3> ParamNames{"Semi-axis 1", "Semi-axis 2", "Orientation"};
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

		static constexpr char const* name() { return "Gaussian mask"; }

		static constexpr auto id() { return ImageProcessorId{"27a6e3a3254aa2a0a8744366c82a363f"}; }

	private:
		ParamMap<InterfaceDescriptor> m_params;
	};
}

#endif
