//@	{
//@	 "targets":[{"name":"dummy_processor.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_MODEL_DUMMYPROCESSOR_HPP
#define TEXPAINTER_MODEL_DUMMYPROCESSOR_HPP

#include "filtergraph/port_info.hpp"
#include "filtergraph/img_proc_arg.hpp"
#include "filtergraph/image_processor_id.hpp"

#include "utils/span_2d.hpp"

namespace Texpainter::Model
{
	class DummyProcessor
	{
	public:
		struct InterfaceDescriptor
		{
			static constexpr std::array<FilterGraph::PortInfo, 0> InputPorts{};
			static constexpr std::array<FilterGraph::PortInfo, 0> OutputPorts{};
			static constexpr std::array<FilterGraph::ParamName, 0> ParamNames{};
		};

		void operator()(FilterGraph::ImgProcArg<InterfaceDescriptor> const&) const {}

		FilterGraph::ParamValue get(FilterGraph::ParamName) const
		{
			return FilterGraph::ParamValue{0.0};
		}

		void set(FilterGraph::ParamName, FilterGraph::ParamValue) {}

		std::span<FilterGraph::ParamValue const> paramValues() const
		{
			return std::span<FilterGraph::ParamValue const>{};
		}

		char const* name() const { return "«Unknown image processor»"; }

		static constexpr auto id()
		{
			return FilterGraph::ImageProcessorId{"419ed08fefebb004dc85ee1d8771330c"};
		}

		static constexpr auto releaseState() { return FilterGraph::ImgProcReleaseState::Stable; }
	};
}

#endif
