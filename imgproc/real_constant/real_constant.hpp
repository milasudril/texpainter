//@	{
//@	 "targets":[{"name":"real_constant.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_IMGPROC_REALCONSTANT_HPP
#define TEXPAINTER_IMGPROC_REALCONSTANT_HPP

#include "filtergraph/proctypes.hpp"
#include "filtergraph/img_proc_arg.hpp"
#include "filtergraph/image_processor_id.hpp"

#include <algorithm>
#include <cmath>

namespace RealConstant
{
	using Texpainter::Str;
	using Texpainter::FilterGraph::ImageProcessorId;
	using Texpainter::FilterGraph::ImgProcArg;
	using Texpainter::FilterGraph::ParamMap;
	using Texpainter::FilterGraph::ParamName;
	using Texpainter::FilterGraph::ParamValue;
	using Texpainter::FilterGraph::PixelType;
	using Texpainter::FilterGraph::PortInfo;
	using Texpainter::FilterGraph::RealValue;

	class ImageProcessor
	{
	public:
		struct InterfaceDescriptor
		{
			static constexpr std::array<PortInfo, 0> InputPorts{};
			static constexpr std::array<PortInfo, 1> OutputPorts{
			    {PixelType::GrayscaleReal, "Output"}};

			static constexpr std::array<ParamName, 1> ParamNames{"Value"};
		};

		void operator()(ImgProcArg<InterfaceDescriptor> const& args) const
		{
			auto const size = args.size().area();
			std::fill(args.output<0>(),
			          args.output<0>() + size,
			          static_cast<float>(m_params.find<Str{"Value"}>()->value()));
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

		static constexpr char const* name() { return "Real constant"; }

		static constexpr auto id() { return ImageProcessorId{"85034d41dbd2669203e0b5046faa7174"}; }

	private:
		ParamMap<InterfaceDescriptor> m_params;
	};
}

#endif
