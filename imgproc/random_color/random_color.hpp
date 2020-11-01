//@	{
//@	 "targets":[{"name":"random_color.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_IMGPROC_RANDOMCOLOR_RANDOMCOLOR_HPP
#define TEXPAINTER_IMGPROC_RANDOMCOLOR_RANDOMCOLOR_HPP

#include "filtergraph/port_info.hpp"
#include "filtergraph/img_proc_arg.hpp"
#include "filtergraph/image_processor_id.hpp"
#include "filtergraph/param_map.hpp"

#include "utils/discrete_pdf.hpp"

namespace RandomColor
{
	using Texpainter::DiscretePdf;
	using Texpainter::FilterGraph::ImageProcessorId;
	using Texpainter::FilterGraph::ImgProcArg;
	using Texpainter::FilterGraph::Palette;
	using Texpainter::FilterGraph::ParamName;
	using Texpainter::FilterGraph::ParamValue;
	using Texpainter::FilterGraph::PortInfo;
	using Texpainter::FilterGraph::PortType;
	using Texpainter::FilterGraph::RealValue;
	using Texpainter::FilterGraph::ParamMap;

	class ImageProcessor
	{
	public:
		struct InterfaceDescriptor
		{
			static constexpr std::array<PortInfo, 2> InputPorts{
			    {PortInfo{PortType::GrayscaleRealPixels, "Random src"},
			     PortInfo{PortType::Palette, "Palette"}}};
			static constexpr std::array<PortInfo, 1> OutputPorts{{PortType::RgbaPixels, "Output"}};

			static constexpr std::array<ParamName, Palette::size()> ParamNames{"0", "1", "2", "3",
				"4", "5", "6", "7",
				"8", "9", "A", "B",
				"C", "D", "E", "F"
			};
		};

		ImageProcessor()
		    : m_pdf{{1.0f,
		            1.0f,
		            1.0f,
		            1.0f,
		            1.0f,
		            1.0f,
		            1.0f,
		            1.0f,
		            1.0f,
		            1.0f,
		            1.0f,
		            1.0f,
		            1.0f,
		            1.0f,
		            1.0f,
		            1.0f}}
		{
			std::ranges::fill(m_params.values(), ParamValue{1.0});
		}

		void operator()(ImgProcArg<InterfaceDescriptor> const& args) const
		{
			auto const size = args.size().area();
			std::transform(
			    args.input<0>(),
			    args.input<0>() + size,
			    args.output<0>(),
			    [&palette = args.input<1>().get(), &pdf = m_pdf](auto val) {
				    return palette[Palette::index_type{static_cast<uint32_t>(pdf.eventIndex(val))}];
			    });
		}

		void set(ParamName name, ParamValue val)
		{
			if(auto ptr = m_params.find(name); ptr != nullptr) [[likely]]
			{
				*ptr = val;
				std::array<float, Palette::size()> weights;
				std::ranges::transform(m_params.values(),
									   std::begin(weights),
									   [](auto val) {return static_cast<float>(val.value());});
				m_pdf = DiscretePdf<float, Palette::size()>{weights};
			}
		}

		ParamValue get(ParamName name) const
		{
			auto ptr = m_params.find(name);
			return ptr != nullptr ? *ptr : ParamValue{0.0};
		}

		std::span<ParamValue const> paramValues() const { return m_params.values(); }

		static constexpr char const* name() { return "Random color"; }

		static constexpr auto id() { return ImageProcessorId{"5c10a297b5c5b898a2f5c2e265caa209"}; }

	private:
		DiscretePdf<float, Palette::size()> m_pdf;
		ParamMap<InterfaceDescriptor> m_params;
	};
}

#endif
