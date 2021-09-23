//@	{
//@	 "targets":[{"name":"topography_sink.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_MODEL_TOPOGRAPHYSINK_HPP
#define TEXPAINTER_MODEL_TOPOGRAPHYSINK_HPP

#include "filtergraph/port_info.hpp"
#include "filtergraph/img_proc_arg.hpp"
#include "filtergraph/image_processor_id.hpp"

#include "utils/span_2d.hpp"

#include <cassert>

namespace Texpainter::Model
{
	class TopographySink
	{
	public:
		struct InterfaceDescriptor
		{
			static constexpr std::array<FilterGraph::PortInfo, 1> InputPorts{
			    {FilterGraph::PortType::TopographyData, "Topography data"}};
			static constexpr std::array<FilterGraph::PortInfo, 0> OutputPorts{};
			static constexpr std::array<FilterGraph::ParamName, 0> ParamNames{};
		};

		void operator()(FilterGraph::ImgProcArg<InterfaceDescriptor> const& args) const
		{
			assert(r_pixels.data() != nullptr);
			std::copy_n(args.input<0>(), area(args.canvasSize()), r_pixels.data());
		}

		FilterGraph::ParamValue get(FilterGraph::ParamName) const
		{
			return FilterGraph::ParamValue{0.0};
		}

		void set(FilterGraph::ParamName, FilterGraph::ParamValue) {}

		std::span<FilterGraph::ParamValue const> paramValues() const
		{
			return std::span<FilterGraph::ParamValue const>{};
		}

		static constexpr char const* name() { return "Output topography"; }

		static constexpr auto id()
		{
			return FilterGraph::ImageProcessorId{"bf710025a9f8c922b30f943533ed9971"};
		}

		static constexpr auto releaseState()
		{
			return FilterGraph::ImgProcReleaseState::Experimental;
		}

		void sink(Span2d<FilterGraph::TopographyInfo> val) { r_pixels = val; }

	private:
		Span2d<FilterGraph::TopographyInfo> r_pixels;
	};
}

#endif
