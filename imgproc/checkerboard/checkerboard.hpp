//@	{
//@	 "targets":[{"name":"checkerboard.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_IMGPROC_CHECKERBOARD_CHECKERBOARD_HPP
#define TEXPAINTER_IMGPROC_CHECKERBOARD_CHECKERBOARD_HPP

#include "filtergraph/port_info.hpp"
#include "filtergraph/img_proc_arg.hpp"
#include "filtergraph/image_processor_id.hpp"

#include <algorithm>

namespace Checkerboard
{
	using Texpainter::Str;
	using Texpainter::FilterGraph::ComplexValue;
	using Texpainter::FilterGraph::ImageProcessorId;
	using Texpainter::FilterGraph::ImgProcArg;
	using Texpainter::FilterGraph::ParamMap;
	using Texpainter::FilterGraph::ParamName;
	using Texpainter::FilterGraph::ParamValue;
	using Texpainter::FilterGraph::PortInfo;
	using Texpainter::FilterGraph::PortType;
	using Texpainter::FilterGraph::RealValue;

	inline auto sizeFromParam(size_t size, GaussianMask2d::ParamValue val)
	{
		return 0.5 * std::exp2(std::lerp(-std::log2(size), 0.0, val.value()));
	}

	class ImageProcessor
	{
	public:
		struct InterfaceDescriptor
		{
			static constexpr std::array<PortInfo, 0> InputPorts{};
			static constexpr std::array<PortInfo, 1> OutputPorts{
			    {PortType::GrayscaleRealPixels, "Intensity"}};

			static constexpr std::array<ParamName, 2> ParamNames{"Div x", "Div y"};
		};

		void operator()(ImgProcArg<InterfaceDescriptor> const& args) const
		{
			auto const w = args.size().width();
			auto const h = args.size().height();
			auto dx = 2.0 * static_cast<int>(w * sizeFromParam(w, *m_params.find<Str("Div x")>()))
			          / static_cast<double>(args.size().width());
			auto dy = 2.0 * static_cast<int>(h * sizeFromParam(h, *m_params.find<Str("Div y")>()))
			          / static_cast<double>(h);

			for(uint32_t row = 0; row < h; ++row)
			{
				auto const i = static_cast<uint32_t>(row * dy);
				for(uint32_t col = 0; col < w; ++col)
				{
					auto const j             = static_cast<uint32_t>(col * dx);
					args.output<0>(col, row) = (i % 2 == j % 2) ? 0.0 : 1.0;
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

		static constexpr char const* name() { return "Checkerboard"; }

		static constexpr auto id() { return ImageProcessorId{"e4d16426a79aa08fce89ff14942e1938"}; }

	private:
		ParamMap<InterfaceDescriptor> m_params;
	};
}

#endif