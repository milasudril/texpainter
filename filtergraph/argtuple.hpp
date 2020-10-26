//@	{
//@	 "targets":[{"name":"argtuple.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_ARGTUPLE_HPP
#define TEXPAINTER_FILTERGRAPH_ARGTUPLE_HPP

#include "./port_info.hpp"
#include "./port_value.hpp"

#include "libenum/tuple.hpp"

#include <array>
#include <type_traits>
#include <cstddef>

namespace Texpainter::FilterGraph
{
	namespace detail
	{
		template<auto types, size_t index = types.size()>
		class GenInArgTuple: public GenInArgTuple<types, index - 1>
		{
		private:
			using type_ = typename PortTypeToType<types[index - 1]>::type;

		public:
			using type = InputPortType<type_>::type;

			constexpr GenInArgTuple() = default;

			template<class... Args>
			constexpr GenInArgTuple(type val, Args&&... args)
			    : GenInArgTuple<types, index - 1>{std::forward<Args>(args)...}
			    , value{val}
			{
			}

			type value;
		};

		template<auto types>
		class GenInArgTuple<types, 0>
		{
		};

		template<auto types, size_t index = types.size()>
		class GenOutArgTuple: public GenOutArgTuple<types, index - 1>
		{
		private:
			using type_ = typename PortTypeToType<types[index - 1]>::type;

		public:
			using type = OutputPortType<type_>::type;

			constexpr GenOutArgTuple() = default;

			template<class... Args>
			constexpr GenOutArgTuple(type val, Args&&... args)
			    : GenOutArgTuple<types, index - 1>{std::forward<Args>(args)...}
			    , value{val}
			{
			}

			type value;
		};

		template<auto types>
		class GenOutArgTuple<types, 0>
		{
		};

		template<class T>
		auto get_output_buffer(T& buffer)
		{
			return std::ref(buffer);
		}

		template<class T>
		auto get_output_buffer(std::unique_ptr<T[]>& buffer)
		{
			return buffer.get();
		}

		template<class T>
		auto get_output_buffer(std::unique_ptr<T>& buffer)
		{
			return std::ref(*buffer);
		}
	}

	template<size_t N>
	constexpr auto portTypes(std::array<PortInfo, N> const& ports)
	{
		std::array<PortType, N> ret{};
		std::ranges::transform(ports, std::begin(ret), [](auto val) { return val.type; });
		return ret;
	}

	template<auto types>
	class InArgTuple
	{
	public:
		template<class... Args>
		constexpr explicit InArgTuple(Args&&... args): m_data{std::forward<Args>(args)...}
		{
		}

		static constexpr auto size() { return types.size(); }

		template<size_t index>
		constexpr auto& get()
		{
			static_assert(index < types.size());
			return static_cast<detail::GenInArgTuple<types, index + 1>&>(m_data).value;
		}

		template<size_t index>
		constexpr auto get() const
		{
			static_assert(index < types.size());
			return static_cast<detail::GenInArgTuple<types, index + 1> const&>(m_data).value;
		}

	private:
		detail::GenInArgTuple<types, types.size()> m_data;
	};

	template<auto types>
	class OutArgTuple
	{
	public:
		template<class... Args>
		constexpr explicit OutArgTuple(Args&&... args): m_data{std::forward<Args>(args)...}
		{
		}

		static constexpr auto size() { return types.size(); }

		template<size_t index>
		constexpr auto& get()
		{
			static_assert(index < types.size());
			return static_cast<detail::GenOutArgTuple<types, index + 1>&>(m_data).value;
		}

		template<size_t index>
		constexpr auto get() const
		{
			static_assert(index < types.size());
			return static_cast<detail::GenOutArgTuple<types, index + 1> const&>(m_data).value;
		}

	private:
		detail::GenOutArgTuple<types, types.size()> m_data;
	};

	template<auto types>
	class OutputBuffers
	{
	public:
		static constexpr auto size() { return types.size(); }

		template<size_t index>
		constexpr void init(Size2d size)
		{
			std::get<index>(m_data) = PortTypeToType<types[index]>::createValue(size);
		}

		template<size_t index>
		constexpr decltype(auto) take()
		{
			return std::move(std::get<index>(m_data));
		}

		template<size_t index>
		constexpr decltype(auto) get()
		{
			return detail::get_output_buffer(std::get<index>(m_data));
		}

	private:
		Enum::TupleFromTypeArray<types, PortTypeToType> m_data;
	};
}

#endif