//@	{
//@	 "targets":[{"name":"gaussian_blur.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"gaussian_blur.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_IMGPROC_GAUSSIANBLUR_GAUSSIANBLUR_HPP
#define TEXPAINTER_IMGPROC_GAUSSIANBLUR_GAUSSIANBLUR_HPP

#include "filtergraph/proctypes.hpp"
#include "filtergraph/img_proc_arg.hpp"
#include "filtergraph/image_processor_id.hpp"
#include "filtergraph/param_map.hpp"
#include "pixel_store/image.hpp"

#include <optional>

namespace GaussianBlur
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
			static constexpr std::array<PortInfo, 1> InputPorts{
			    {PortInfo{PixelType::GrayscaleComplex, "Image spectrum"}}};
			static constexpr std::array<PortInfo, 1> OutputPorts{
			    {PortInfo{PixelType::GrayscaleComplex, "Image spectrum"}}};

			static constexpr std::array<ParamName, 3> ParamNames{"ξ_0", "η_0", "θ"};
		};

		void operator()(ImgProcArg<InterfaceDescriptor> const& args) const;

		void set(ParamName name, ParamValue value)
		{
			if(auto ptr = m_params.find(name); ptr != nullptr) [[likely]]
				{
					*ptr     = value;
					m_kernel = {};
				}
		}

		ParamValue get(ParamName name) const
		{
			auto ptr = m_params.find(name);
			return ptr != nullptr ? *ptr : ParamValue{0.0};
		}

		std::span<ParamValue const> paramValues() const { return m_params.values(); }

		static constexpr char const* name() { return "Gaussian blur"; }

		static constexpr auto id() { return ImageProcessorId{"1041A52FDD679E35C7916ED99067B5DB"}; }

	private:
		mutable std::optional<BasicImage<RealValue>> m_kernel;
		ParamMap<InterfaceDescriptor> m_params;
	};
}

#endif
