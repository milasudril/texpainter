//@	{
//@	 "targets":[{"name":"argtuple.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_ARGTUPLE_HPP
#define TEXPAINTER_FILTERGRAPH_ARGTUPLE_HPP

#include "./port_info.hpp"
#include "./port_value.hpp"
#include "./node_argument.hpp"

#include "utils/create_tuple.hpp"

#include "libenum/tuple.hpp"

#include <array>
#include <type_traits>
#include <cstddef>

namespace Texpainter::FilterGraph
{
	namespace detail
	{
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

	namespace detail
	{
		template<PortType t>
		struct GenInputPortType
		{
			using type = typename InputPortType<typename PortTypeToType<t>::type>::type;
		};
	}

	template<class T>
	void doStuffWithTArg(T);

	template<class T>
	void doStuffWithT();


	template<auto types>
	class InArgTuple
	{
		using storage_type = typename Enum::TupleFromTypeArray<types, detail::GenInputPortType>;

	public:
		constexpr InArgTuple() = default;

		constexpr explicit InArgTuple(NodeArgument const& args)
		    : m_data{createTuple<storage_type>([&inputs = args.inputs()]<class Tag>(Tag) {
			    using InputT  = typename detail::GenInputPortType<types[Tag::value]>::type;
			    using OutputT = std::tuple_element_t<Tag::value, storage_type>;
			    static_assert(std::is_same_v<InputT, OutputT>);
			    return *Enum::get_if<InputT>(&inputs[Tag::value]);
		    })}
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
	using OutArgTuple = Enum::TupleFromTypeArray<types, detail::GenOutputPortType>;
}

#endif