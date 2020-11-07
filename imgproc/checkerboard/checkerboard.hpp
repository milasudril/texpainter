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
	using Texpainter::vec2_t;
	using Texpainter::FilterGraph::ComplexValue;
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
			    {PortType::GrayscaleRealPixels, "Intensity"}};

			static constexpr std::array<ParamName, 2> ParamNames{"Div x", "Div y"};
		};

		void operator()(ImgProcArg<InterfaceDescriptor> const& args) const
		{
			auto dx = 2.0
			          * static_cast<int>(0.5 * args.size().width()
			                             * m_params.find<Str("Div x")>()->value())
			          / static_cast<double>(args.size().width());
			auto dy = 2.0
			          * static_cast<int>(0.5 * args.size().height()
			                             * m_params.find<Str("Div y")>()->value())
			          / static_cast<double>(args.size().height());

			for(uint32_t row = 0; row < args.size().height(); ++row)
			{
				auto const i = static_cast<uint32_t>(row * dy);
				for(uint32_t col = 0; col < args.size().width(); ++col)
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
