//@	{
//@	 "targets":[{"name":"argtuple.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_ARGTUPLE_HPP
#define TEXPAINTER_FILTERGRAPH_ARGTUPLE_HPP

#include "./port_info.hpp"
#include "./port_value.hpp"

#include "utils/create_tuple.hpp"

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
	class OutputBuffers
	{
		using storage_type = Enum::TupleFromTypeArray<types, PortTypeToType>;

	public:
		explicit OutputBuffers(Size2d size)
		    : m_data{createTuple<storage_type>(
		        [size]<class T>(T) { return PortTypeToType<types[T::value]>::createValue(size); })}
		{
		}

		static constexpr auto size() { return types.size(); }

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
		storage_type m_data;
	};

	namespace detail
	{
		template<PortType t>
		struct GenOutputPortType
		{
			using type = OutputPortType<typename PortTypeToType<t>::type>::type;
		};
	}

	template<auto types>
	class OutArgTuple
	{
		using storage_type = Enum::TupleFromTypeArray<types, detail::GenOutputPortType>;

	public:
		OutArgTuple() = default;

		constexpr explicit OutArgTuple(OutputBuffers<types>& buffers)
		    : m_data{createTuple<storage_type>(
		        [&buffers]<class T>(T) { return buffers.template get<T::value>(); })}
		{
		}

		static constexpr auto size() { return types.size(); }

		template<size_t index>
		constexpr auto get() const
		{
			static_assert(index < types.size());
			return std::get<index>(m_data);
		}

	private:
		storage_type m_data;
	};
}

#endif