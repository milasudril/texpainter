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
		static constexpr std::string_view s_input_port_names[] = {"Pixels"};

		std::span<std::string_view const> paramNames() const
		{
			return std::span<std::string_view const>{};
		}

		std::span<ProcParamValue const> paramValues() const
		{
			return std::span<ProcParamValue const>{};
		}

		void set(std::string_view, ProcParamValue) {}

		ProcParamValue get(std::string_view) const { return ProcParamValue{0.0}; }

		static constexpr std::span<std::string_view const> inputPorts()
		{
			return std::span<std::string_view const>{s_input_port_names, 1};
		}

		static constexpr std::span<std::string_view const> outputPorts()
		{
			return std::span<std::string_view const>{};
		}

		std::vector<ProcResultType> operator()(std::span<ProcArgumentType const> args) const
		{
			if(args.size() != 1) [[unlikely]]
				{
					return std::vector<ProcResultType>{};
				}

			if(auto args_0 = std::get_if<Span2d<PixelStore::Pixel const>>(&args[0]);
			   args_0 != nullptr)
				[[likely]] { return std::vector<ProcResultType>{PixelStore::Image{*args_0}}; }

			return std::vector<ProcResultType>{};
		}

		static constexpr std::string_view name() { return "Layer output"; }
	};
}

#endif
