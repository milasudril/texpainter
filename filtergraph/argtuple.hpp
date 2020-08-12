//@	{
//@	 "targets":[{"name":"argtuple.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_ARGTUPLE_HPP
#define TEXPAINTER_FILTERGRAPH_ARGTUPLE_HPP

#include "./proctypes.hpp"

#include <array>
#include <type_traits>
#include <cstddef>

namespace Texpainter::FilterGraph
{
	namespace detail
	{
		template<auto types, size_t index = types.size()>
		struct GenArgTuple: GenArgTuple<types, index - 1>
		{
			using type = typename PixelTypeToType<types[index - 1]>::type;
			std::add_pointer_t<std::add_const_t<type>> value;
		};

		template<auto types>
		struct GenArgTuple<types, 0>
		{
		};
	}

	template<auto types>
	using ArgTuple = detail::GenArgTuple<types, types.size()>;


	template<size_t index, auto types>
	auto& get(ArgTuple<types>& t)
	{
		return static_cast<detail::GenArgTuple<types, index + 1>&>(t).value;
	}

	template<size_t index, auto types>
	auto get(ArgTuple<types> const& t)
	{
		return static_cast<detail::GenArgTuple<types, index + 1> const&>(t).value;
	}
}

#endif