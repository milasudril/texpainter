//@	{
//@	 "targets":[{"name":"node_argument.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_NODEARGUMENT_HPP
#define TEXPAINTER_FILTERGRAPH_NODEARGUMENT_HPP

#include "utils/size_2d.hpp"

#include <array>
#include <functional>

namespace Texpainter::FilterGraph
{
	namespace detail
	{
		template<class T>
		void do_cast(T& pointer, void const* other)
		{
			pointer = reinterpret_cast<std::decay_t<T>>(other);
		}

		template<class T>
		void do_cast(T& pointer, void* other)
		{
			pointer = reinterpret_cast<std::decay_t<T>>(other);
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
		static constexpr size_t MaxNumOutputs = 4;

		explicit NodeArgument(Size2d size,
			std::array<void const*, MaxNumInputs> const& inputs,
			std::array<void*, MaxNumOutputs> const& outputs
 							)
		    : m_size{size}
		    , r_inputs{inputs}
		    , r_outputs{outputs}
		{
		}

		template<class T>
		auto inputs() const
		{
			static_assert(T::size() <= MaxNumInputs);
			T ret{};
			detail::apply(
			    [this, index = 0](auto&... args) mutable {
				    (..., detail::do_cast(args, r_inputs[index++]));
			    },
			    ret);
			return ret;
		}

		template<class T>
		auto outputs() const
		{
			static_assert(T::size() <= MaxNumInputs);
			T ret{};
			detail::apply(
			    [this, index = 0](auto&... args) mutable {
				    (..., detail::do_cast(args, r_outputs[index++]));
			    },
			    ret);
			return ret;
		}

		Size2d size() const { return m_size; }

	private:
		Size2d m_size;
		std::array<void const*, MaxNumInputs> r_inputs;
		std::array<void*, MaxNumOutputs> r_outputs;
	};
}

#endif