//@	{
//@	 "targets":[{"name":"node_argument.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_NODEARGUMENT_HPP
#define TEXPAINTER_FILTERGRAPH_NODEARGUMENT_HPP

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
		template<class... ArgTypes>
		auto inputs() const
		{
			static_assert(sizeof...(ArgTypes) <= 4);
			std::tuple<std::add_pointer_t<std::add_const_t<ArgTypes>>...> ret{};
			std::apply(
			    [this, index = 0](auto&... args) mutable {
				    (..., detail::do_cast(args, r_inputs[index++]));
			    },
			    ret);
			return ret;
		}
		std::array<void const*, 4> r_inputs;

	private:
	};
}

#endif