//@	{
//@	 "targets":[{"name":"abstract_image_processor.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_ABSTRACTIMAGEPROCESSOR_HPP
#define TEXPAINTER_FILTERGRAPH_ABSTRACTIMAGEPROCESSOR_HPP

#include "./node_argument.hpp"
#include "./proctypes.hpp"

#include "utils/memblock.hpp"

namespace Texpainter::FilterGraph
{
	class AbstractImageProcessor
	{
	public:
		static constexpr size_t MaxNumOutputs = 4;

		virtual std::array<Memblock, MaxNumOutputs> operator()(NodeArgument const& arg) const = 0;
		virtual std::span<PortInfo const> inputPorts() const                                  = 0;
		virtual std::span<PortInfo const> outputPorts() const                                 = 0;
		virtual std::span<ParamName const> paramNames() const                                 = 0;
		virtual std::span<ParamValue const> paramValues() const                               = 0;
		virtual ParamValue get(std::string_view param_name) const                             = 0;
		virtual AbstractImageProcessor& set(std::string_view param_name, ParamValue value)    = 0;
		virtual std::unique_ptr<AbstractImageProcessor> clone() const                         = 0;
		virtual char const* name() const                                                      = 0;
		virtual ~AbstractImageProcessor() = default;
	};
}

#endif