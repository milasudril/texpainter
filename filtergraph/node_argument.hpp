//@	{
//@	 "targets":[{"name":"node_argument.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_NODEARGUMENT_HPP
#define TEXPAINTER_FILTERGRAPH_NODEARGUMENT_HPP

#include "./port_value.hpp"

#include "utils/size_2d.hpp"

#include <array>
#include <functional>

namespace Texpainter::FilterGraph
{
	namespace detail
	{
		template<class T, class VariantType>
		void extract(T& result, VariantType const& val)
		{
			using std::get_if;
			result = *Enum::get_if<T>(&val);
		}

		template<class F, class Tuple, std::size_t... I>
		constexpr decltype(auto) apply_impl(F&& f, Tuple&& t, std::index_sequence<I...>)
		{
			return std::invoke(std::forward<F>(f), t.template get<I>()...);
		}

		template<class F, class Tuple>
		constexpr decltype(auto) apply(F&& f, Tuple&& t)
		{
			return detail::apply_impl(
			    std::forward<F>(f),
			    std::forward<Tuple>(t),
			    std::make_index_sequence<std::remove_reference_t<Tuple>::size()>{});
		}
	}

	class NodeArgument
	{
	public:
		static constexpr size_t MaxNumInputs = 4;
		explicit NodeArgument(Size2d size, std::array<InputPortValue, MaxNumInputs> const& inputs)
		    : m_size{size}
		    , r_inputs{inputs}
		{
		}

		template<class T>
		auto inputs() const
		{
			static_assert(T::size() <= MaxNumInputs);
			T ret{};
			detail::apply(
			    [this, index = 0](auto&... args) mutable {
				    (..., detail::extract(args, r_inputs[index++]));
			    },
			    ret);
			return ret;
		}

		Size2d size() const { return m_size; }

	private:
		Size2d m_size;
		std::array<InputPortValue, MaxNumInputs> r_inputs;
	};
}

#endif