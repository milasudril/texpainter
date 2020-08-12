//@	{
//@	 "targets":[{"name":"node_argument.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_NODEARGUMENT_HPP
#define TEXPAINTER_FILTERGRAPH_NODEARGUMENT_HPP

#include "utils/size_2d.hpp"

#include <array>
#include <tuple>
#include <cstdio>

namespace Texpainter::FilterGraph
{
	namespace detail
	{
		template<class T>
		void do_cast(T& pointer, void const* other)
		{
			pointer = reinterpret_cast<std::decay_t<T>>(other);
		}
	}

	class NodeArgument
	{
	public:
		static constexpr size_t MaxNumInputs = 4;

		explicit NodeArgument(Size2d size, std::array<void const*, MaxNumInputs> const& inputs)
		    : m_size{size}
		    , r_inputs{inputs}
		{
		}

		template<class T>
		auto inputs() const
		{
			T ret{};
			std::apply(
			    [this, index = 0](auto&... args) mutable {
				    (..., detail::do_cast(args, r_inputs[index++]));
			    },
			    ret);
			return ret;
		}

		Size2d size() const { return m_size; }

	private:
		Size2d m_size;
		std::array<void const*, MaxNumInputs> r_inputs;
	};
}

#endif