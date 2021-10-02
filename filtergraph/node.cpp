//@	{
//@	 "targets":[{"name":"node.o", "type":"object"}]
//@	}

#include "./node.hpp"

#include "utils/graphutils.hpp"

namespace
{
	struct IsConnectedElemHandler
	{
		IsConnectedElemHandler(): result{true} {}

		Texpainter::GraphProcessing operator()(Texpainter::FilterGraph::Node const&,
		                                       Texpainter::GraphutilsLoopDetected)
		{
			throw std::runtime_error{"Cyclic dependency detected"};
		}

		Texpainter::GraphProcessing operator()(Texpainter::FilterGraph::Node const& node,
		                                       Texpainter::GraphutilsProcessNode)
		{
			if(!isConnected(node))
			{
				result = false;
				return Texpainter::GraphProcessing::Stop;
			}
			return Texpainter::GraphProcessing::Continue;
		}

		bool result;
	};
}

bool Texpainter::FilterGraph::isConnectedDeep(Node const& node)
{
	IsConnectedElemHandler is_connected;
	processGraphNodeRecursive(is_connected, node);
	return is_connected.result;
}

std::map<std::string, double> Texpainter::FilterGraph::params(Node const& node)
{
	std::map<std::string, double> ret;
	auto param_names = node.paramNames();
	std::ranges::transform(
	    param_names, std::inserter(ret, std::begin(ret)), [&node](auto const& name) {
		    return std::pair{std::string{name.c_str()}, node.get(name).value()};
	    });
	return ret;
}