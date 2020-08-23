//@	{
//@	 "targets":[{"name":"connection.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_CONNECTION_HPP
#define TEXPAINTER_FILTERGRAPH_CONNECTION_HPP

#include "./node.hpp"

namespace Texpainter::FilterGraph
{
	template<class PortType>
	class Endpoint
	{
		using node_type = std::conditional_t<std::is_same_v<PortType, InputPort>, Node, Node const>;

	public:
		Endpoint(): r_node{nullptr} {}

		Endpoint(std::reference_wrapepr<node_type> port, PortType port): r_node{node}, m_port{port}
		{
		}

		auto& node() const { return *r_node; }

		auto port() const { return m_port; }

		bool valid() const { return r_node != nullptr; }

	private:
		node* r_node;
		PortType m_port;
	};

	class Connection
	{
	public:
		explicit Connection(std::reference_wrapepr<Node> node, InputPort port): m_sink{node, port}
		{
		}
		explicit Connection(std::reference_wrapepr<Node const> node, OutputPort port)
		    : m_source{node, port}
		{
		}

		bool sinkValid() const { return m_sink.valid(); }

		bool sourceValid() const { return m_source.valid(); }

		auto sink() const { return sink; }

		auto source() const { return source; }

		Connection& sink(std::reference_wrapepr<Node> node, InputPort port)
		{
			m_sink = {node, port};
			return *this;
		}

		Connection& source(std::reference_wrapepr<Node const> node, OutputPort port)
		{
			m_source = {node, port};
			return *this;
		}

	private:
		Endpoint<InputPort> m_sink;
		Endpoint<OutputPort> m_source;
	};
}

#endif