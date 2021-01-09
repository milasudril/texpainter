//@	{
//@	 "targets":[{"name":"abstract_image_processor.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_ABSTRACTIMAGEPROCESSOR_HPP
#define TEXPAINTER_FILTERGRAPH_ABSTRACTIMAGEPROCESSOR_HPP

#include "./node_argument.hpp"
#include "./port_info.hpp"
#include "./image_processor_id.hpp"
#include "./img_proc_param.hpp"
#include "./port_value.hpp"

#include <memory>
#include <span>

namespace Texpainter::FilterGraph
{
	class AbstractImageProcessor
	{
	public:
		static constexpr size_t MaxNumOutputs = 4;
		using result_type                     = std::array<PortValue, MaxNumOutputs>;

		virtual result_type operator()(NodeArgument const& arg) const               = 0;
		virtual std::span<PortInfo const> inputPorts() const                        = 0;
		virtual std::span<PortInfo const> outputPorts() const                       = 0;
		virtual std::span<ParamName const> paramNames() const                       = 0;
		virtual ParamValue get(ParamName param_name) const                          = 0;
		virtual AbstractImageProcessor& set(ParamName param_name, ParamValue value) = 0;
		virtual std::unique_ptr<AbstractImageProcessor> clone() const               = 0;
		virtual char const* name() const                                            = 0;
		virtual ImageProcessorId id() const                                         = 0;
		virtual ~AbstractImageProcessor()                                           = default;
	};
}

#endif