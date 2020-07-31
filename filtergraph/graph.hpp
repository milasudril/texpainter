//@	{
//@	 "targets":[{"name":"graph.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_GRAPH_HPP
#define TEXPAINTER_FILTERGRAPH_GRAPH_HPP

#include "./processor_node.hpp"
#include "./node_id.hpp"
#include "./image_source.hpp"
#include "./output_node.hpp"

#include "utils/iter_pair.hpp"

#include <algorithm>

namespace Texpainter::FilterGraph
{
	class Graph
	{
	public:
		Graph(): m_output_node{OutputNode{}}
		{
			m_output_node.connect(InputPort{0}, m_input_node, OutputPort{0});
		}

		Graph(Graph const& other)
		{
			std::ranges::for_each(other.m_nodes, [&result = m_nodes](auto const& item) {
				result.insert(std::make_pair(item.first, item.second.disconnectedCopy()));
			});

			std::map<ProcessorNode const*, NodeId> ptr_to_id;
			std::ranges::transform(
			    other.m_nodes, std::inserter(ptr_to_id, std::end(ptr_to_id)), [](auto const& item) {
				    return std::make_pair(&item.second, item.first);
			    });

			std::ranges::for_each(
			    other.m_nodes,
			    [&result = m_nodes, result_iter = std::begin(m_nodes), &ptr_to_id](
			        auto const& item) mutable {
				    std::ranges::for_each(
				        item.second.inputs(),
				        [&result, &result_iter, &ptr_to_id, k = 0u](auto const& connection) mutable {
					        if(auto i = ptr_to_id.find(&connection.processor());
					           i != std::end(ptr_to_id))
					        {
						        auto& node = result.find(i->second)->second;
						        result_iter->second.connect(InputPort{k}, node, connection.port());
					        }
					        ++k;
				        });
				    ++result_iter;
			    });
		}

		template<class PixelType>
		PixelStore::Image process(Span2d<PixelType const> source) const
		{
			m_input_node.replaceWith(ImageSource{source});
			auto ret = m_output_node();
			if(ret.size() != 1) [[unlikely]]  // Should only have one output
				{
					PixelStore::Image ret{source.size()};
					std::ranges::fill(ret.pixels(), PixelStore::Pixel{0.0f, 0.0f, 0.0f, 0.0f});
				}

			// NOTE: OutputNode always returns PixelStore::Image
			return *std::get_if<PixelStore::Image>(&ret[0]);
		}

		Graph& connectInput(NodeId node, InputPort sink)
		{
			m_nodes[node].connect(sink, m_input_node, OutputPort{0});
			return *this;
		}

		Graph& connectOutput(NodeId node, OutputPort src)
		{
			m_output_node.connect(InputPort{0}, m_nodes[node], src);
			return *this;
		}

		Graph& connect(NodeId a, InputPort sink, NodeId b, OutputPort src)
		{
			m_nodes[a].connect(sink, m_nodes[b], src);
			return *this;
		}


		Graph& disconnectOutput()
		{
			m_output_node.disconnect(InputPort{0});
			return *this;
		}

		Graph& disconnect(NodeId a, InputPort sink)
		{
			m_nodes[a].disconnect(sink);
			return *this;
		}


		template<ImageProcessor T>
		NodeId insert(T&& obj)
		{
			auto i = m_nodes.insert(std::make_pair(m_current_id, std::forward<T>(obj)));
			++m_current_id;
			return i.first->first;
		}

		auto nodes() const { return IterPair{std::begin(m_nodes), std::end(m_nodes)}; }

		auto get(NodeId id, std::string_view paramname) const
		{
			// Assume that id exists
			return m_nodes.find(id)->second.get(paramname);
		}

		void set(NodeId id, std::string_view paramname, ProcParamValue val)
		{
			// Assume that id exists
			m_nodes.find(id)->second.set(paramname, val);
		}

		auto currentId() const { return m_current_id; }

	private:
		mutable ProcessorNode m_input_node;
		ProcessorNode m_output_node;
		std::map<NodeId, ProcessorNode> m_nodes;
		NodeId m_current_id;
	};
}

#endif