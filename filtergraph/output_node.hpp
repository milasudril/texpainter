//@	{
//@	 "targets":[{"name":"output_node.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_OUTPUTNODE_HPP
#define TEXPAINTER_FILTERGRAPH_OUTPUTNODE_HPP

#include "./proctypes.hpp"

#include <string_view>
#include <vector>

namespace Texpainter::FilterGraph
{
	class OutputNode
	{
	public:
		std::span<std::string_view const> paramNames() const
		{
			return std::span<std::string_view const>{};
		}

		std::vector<ProcParamValue> paramValues() const { return std::vector<ProcParamValue>{}; }

		void set(std::string_view, ProcParamValue) {}

		ProcParamValue get(std::string_view) const { return ProcParamValue{0.0}; }

		std::span<std::string_view const> inputPorts() const
		{
			constexpr std::string_view ret[] = {"Output"};
			return std::span<std::string_view const>{ret, 1};
		}

		std::span<std::string_view const> outputPorts() const
		{
			return std::span<std::string_view const>{};
		}

		std::vector<ProcResultType> operator()(std::span<ProcArgumentType const> args) const
		{
			if(args.size() != 1) [[unlikely]]
				{
					return std::vector<ProcResultType>{Model::Image{Size2d{1, 1}}};
				}

			if(auto args_0 = std::get_if<Span2d<Model::Pixel const>>(&args[0]); args_0 != nullptr)
				[[likely]] { return std::vector<ProcResultType>{Model::Image{*args_0}}; }

			return std::vector<ProcResultType>{Model::Image{Size2d{1, 1}}};
		}
	};
}

#endif
