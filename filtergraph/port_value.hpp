//@	{
//@	 "targets":[{"name":"port_value.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_PORTVALUE_HPP
#define TEXPAINTER_FILTERGRAPH_PORTVALUE_HPP

#include "./port_type.hpp"

#include "utils/size_2d.hpp"

#include "libenum/variant.hpp"


namespace Texpainter::FilterGraph
{
	namespace detail
	{
		template<class T>
		concept SmartPointer = requires
		{
			T::element_type;
		};

		template<class T>
		requires(!SmartPointer<T>) struct MakeInputType
		{
			using type = std::conditional_t<(sizeof(T) <= 16), T, T const*>;
		};

		template<class T>
		struct MakeInputType<std::unique_ptr<T[]>>
		{
			using type = T const*;
		};

		template<class T>
		struct MakeInputType<std::unique_ptr<T>>
		{
			using type = T const*;
		};

		template<class T>
		auto makeInputPortValue(T const& val) requires(sizeof(T) <= 16)
		{
			return val;
		}

		template<class T>
		T const* makeInputPortValue(std::unique_ptr<T[]> const& val)
		{
			return val.get();
		}

		template<class T>
		T const* makeInputPortValue(std::unique_ptr<T> const& val)
		{
			return val.get();
		}

		template<class T>
		auto makeInputPortValue(T const& val) requires(sizeof(T) > 16)
		{
			return &val;
		}
	}

	template<PortType t>
	struct MakeInputPortValue
	{
		using type = typename detail::MakeInputType<typename PortTypeToType<t>::type>::type;
	};

	using PortValue      = Enum::Variant<PortType, PortTypeToType>;
	using InputPortValue = Enum::Variant<PortType, MakeInputPortValue>;

	inline InputPortValue makeInputPortValue(PortValue const& val)
	{
		using std::visit;

		return visit(
		    [](auto const& val) { return InputPortValue{detail::makeInputPortValue(val)}; }, val);
	}
}
#endif