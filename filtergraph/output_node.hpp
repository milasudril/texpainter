//@	{
//@	 "targets":[{"name":"output_node.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_OUTPUTNODE_HPP
#define TEXPAINTER_FILTERGRAPH_OUTPUTNODE_HPP

#include "./proctypes.hpp"

#include <vector>

namespace Texpainter::FilterGraph
{
	class OutputNode
	{
		static constexpr PortInfo s_input_ports[] = {{PixelType::RGBA, "Pixels"}};

	public:
		static constexpr std::span<char const* const> paramNames()
		{
			return std::span<char const* const>{};
		}

		std::span<ProcParamValue const> paramValues() const
		{
			return std::span<ProcParamValue const>{};
		}

		void set(std::string_view, ProcParamValue) {}

		ProcParamValue get(std::string_view) const { return ProcParamValue{0.0}; }

		static constexpr std::span<PortInfo const> inputPorts()
		{
			return std::span<PortInfo const>{s_input_ports};
		}

		static constexpr std::span<PortInfo const> outputPorts()
		{
			return std::span<PortInfo const>{};
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

		static constexpr char const* name() { return "Layer output"; }
	};
}

#endif
