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
		struct GenInArgTuple: GenInArgTuple<types, index - 1>
		{
			using type = typename PixelTypeToType<types[index - 1]>::type;
			std::add_pointer_t<std::add_const_t<type>> value;
		};

		template<auto types>
		struct GenInArgTuple<types, 0>
		{
		};

		template<auto types, size_t index = types.size()>
		struct GenOutArgTuple: GenOutArgTuple<types, index - 1>
		{
			using type = typename PixelTypeToType<types[index - 1]>::type;
			std::add_pointer_t<type> value;
		};

		template<auto types>
		struct GenOutArgTuple<types, 0>
		{
		};
	}

	template<auto types>
	class InArgTuple
	{
	public:
		static constexpr auto size() { return types.size(); }

		template<size_t index>
		constexpr auto& get()
		{
			return static_cast<detail::GenInArgTuple<types, index + 1>&>(m_data).value;
		}

		template<size_t index>
		constexpr auto get() const
		{
			return static_cast<detail::GenInArgTuple<types, index + 1> const&>(m_data).value;
		}

	private:
		detail::GenInArgTuple<types, types.size()> m_data;
	};

	template<auto types>
	class OutArgTuple
	{
	public:
		static constexpr auto size() { return types.size(); }

		template<size_t index>
		constexpr auto& get()
		{
			return static_cast<detail::GenOutArgTuple<types, index + 1>&>(m_data).value;
		}

		template<size_t index>
		constexpr auto get() const
		{
			return static_cast<detail::GenOutArgTuple<types, index + 1> const&>(m_data).value;
		}

	private:
		detail::GenOutArgTuple<types, types.size()> m_data;
	};
}

#endif