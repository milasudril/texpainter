//@	{
//@	 "targets":[{"name":"port_value.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_FILTERGRAPH_PORTVALUE_HPP
#define TEXPAINTER_FILTERGRAPH_PORTVALUE_HPP

#include "./port_type.hpp"

#include "utils/size_2d.hpp"

#include "libenum/variant.hpp"

#include <functional>


namespace Texpainter::FilterGraph
{
	namespace detail
	{
		template<class T>
		concept SmartPointer = requires
		{
			T::element_type;
		};
	}

	template<class T>
	requires(!detail::SmartPointer<T>) struct InputPortType
	{
		using type = std::conditional_t<(sizeof(T) <= 16), T, std::reference_wrapper<T const>>;
	};

	template<class T>
	struct InputPortType<std::unique_ptr<T[]>>
	{
		using type = T const*;
	};

	template<class T>
	struct InputPortType<std::unique_ptr<T>>
	{
		using type = std::reference_wrapper<T const>;
	};

	template<class T>
	struct OutputPortType
	{
		using type = std::reference_wrapper<T>;
	};

	template<class T>
	struct OutputPortType<std::unique_ptr<T[]>>
	{
		using type = T*;
	};

	template<class T>
	struct OutputPortType<std::unique_ptr<T>>
	{
		using type = std::reference_wrapper<T>;
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
		return std::ref(val);
	}

	namespace detail
	{
		template<PortType t>
		struct MakeInputPortValue
		{
			using type = typename InputPortType<typename PortTypeToType<t>::type>::type;
		};
	}

	using PortValue      = Enum::Variant<PortType, PortTypeToType>;
	using InputPortValue = Enum::Variant<PortType, detail::MakeInputPortValue>;

	inline InputPortValue makeInputPortValue(PortValue const& val)
	{
		using std::visit;

		return visit([](auto const& val) { return InputPortValue{makeInputPortValue(val)}; }, val);
	}
}
#endif