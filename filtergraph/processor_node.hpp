//@	{
//@	 "targets":[{"name":"processor_node.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERPIPE_PROCESSORNODE_HPP
#define TEXPAINTER_FILTERPIPE_PROCESSORNODE_HPP

#include "model/image.hpp"
#include "utils/unique_function.hpp"

#include <complex>
#include <vector>
#include <variant>

namespace Texpainter::FilterPipe
{
	class ProcessorNode
	{
	public:
		using argument_type = std::variant<std::false_type,
		                                   Span2d<Model::Pixel const>,
		                                   Span2d<double>,
		                                   Span2d<std::complex<double>>>;

		using result_type = std::variant<Model::Image,
		                                 Model::BasicImage<double>,
		                                 Model::BasicImage<std::complex<double>>>;


		ProcessorNode()
		    : m_proc{[](std::span<argument_type const>) {
			    return std::vector<result_type> {}
		    }}
		{
		}

		template<class Processor,
		         std::enable_if_t<!std::is_same_v<ProcessorNode, std::decay_t<Processor>>, int> = 0>
		explicit ProcessorNode(Processor&& proc)
		    : m_inputs(proc.inputCount())  // Must use old-style ctor here to get the correct size
		    , m_proc{std::forward<Processor>(proc)}
		{
		}


		std::vector<return_type> const& operator()() const
		{
			if(m_result.size() != 0) { return m_result; }

			std::vector<argument_type> args;
			std::ranges::transform(
			    m_inputs, std::back_inserter(args), [](auto const& val) { return val(); });

			m_result_cache = m_proc(args);
			return m_result_cache;
		}


		ProcessorNode& connect(size_t socket,
		                       std::shared_ptr<ProcessorNode const>&& other,
		                       size_t other_socket)
		{
			m_inputs[socket] = SourceNode{std::move(other), other_socket};
			return *this;
		}

		ProcessorNode& disconnect(size_t socket)
		{
			m_inputs[socket] = SourceNode{};
			return *this;
		}


	private:
		mutable std::vector<return_type> m_result_cache;

		class SourceNode
		{
		public:
			SourceNode(): m_processor{std::make_shared<ProcessorNode>(), 0} {}

			decltype(auto) operator()() const
			{
				if(auto res = (*m_processor)(); m_index < std::size(res)) [[likely]]
					{
						return argument_type{
						    std::visit([](auto const& val) { return val.pixels(); }, res[m_index])};
					}

				return argument_type{std::false_type{}};
			}

		private:
			std::shared_ptr<ProcessorNode const> m_processor;
			size_t m_index;
		};

		std::vector<SourceNode> m_inputs;
		UniqueFunction m_proc;
	};
}

#endif