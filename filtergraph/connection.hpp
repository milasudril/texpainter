//@	{
//@	 "targets":[{"name":"connection.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_CONNECTION_HPP
#define TEXPAINTER_FILTERGRAPH_CONNECTION_HPP

#include "./node.hpp"

namespace Texpainter::FilterGraph
{
	template<class PortDirection>
	class Endpoint
	{
		using node_type =
		    std::conditional_t<std::is_same_v<PortDirection, InputPortIndex>, Node, Node const>;

	public:
		Endpoint(): r_node{nullptr}, m_port{0} {}

		Endpoint(std::reference_wrapper<node_type> node, PortDirection port)
		    : r_node{&node.get()}
		    , m_port{port}
		{
		}

		auto& node() const { return *r_node; }

		auto port() const { return m_port; }

		bool valid() const { return r_node != nullptr; }

	private:
		node_type* r_node;
		PortDirection m_port;
	};

	class Connection
	{
	public:
		explicit Connection(std::reference_wrapper<Node> node, InputPortIndex port)
		    : m_sink{node, port}
		{
		}

		explicit Connection(std::reference_wrapper<Node const> node, OutputPortIndex port)
		    : m_source{node, port}
		{
		}

		bool sinkValid() const { return m_sink.valid(); }

		bool sourceValid() const { return m_source.valid(); }

		auto sink() const { return m_sink; }

		auto source() const { return m_source; }

		Connection& sink(std::reference_wrapper<Node> node, InputPortIndex port)
		{
			m_sink = {node, port};
			return *this;
		}

		Connection& source(std::reference_wrapper<Node const> node, OutputPortIndex port)
		{
			m_source = {node, port};
			return *this;
		}

	private:
		Endpoint<InputPortIndex> m_sink;
		Endpoint<OutputPortIndex> m_source;
	};

	template<class ValidationCallback>
	inline auto validate(Connection const& conn, ValidationCallback&& cb)
	{
		if(!conn.sink().valid() || !conn.source().valid()) [[unlikely]]
			{
				return cb.notCompleted(conn);
			}


		if(&conn.sink().node() == &conn.source().node()) { return cb.selfConnection(conn); }

		if(conn.sink().node().inputPorts()[conn.sink().port().value()].type
		   != conn.source().node().outputPorts()[conn.source().port().value()].type)
		{ return cb.typeMismatch(conn); }

		return cb.connectionOk(conn);
	}

	inline void establish(Connection const& conn)
	{
		conn.sink().node().connect(conn.sink().port(), conn.source().node(), conn.source().port());
	}
}

#endif