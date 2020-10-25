//@	{
//@	 "targets":[{"name":"node_argument.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_UTILS_CREATETUPLE_HPP
#define TEXPAINTER_UTILS_CREATETUPLE_HPP

#include <tuple>
#include <type_traits>

namespace Texpainter
{
	namespace detail
	{
		template<class Object, class F, size_t... I>
		constexpr decltype(auto) create_impl(F&& f, std::index_sequence<I...>)
		{
			return Object{f(std::integral_constant<size_t, I>{})...};
		}
	}

	template<class Tuple, class F, size_t... I>
	constexpr decltype(auto) createTuple(F&& f)
	{
		return detail::create_impl<Tuple>(std::forward<F>(f),
		                                  std::make_index_sequence<std::tuple_size_v<Tuple>>{});
	}
}

#endif