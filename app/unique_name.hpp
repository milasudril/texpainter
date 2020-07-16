//@	{
//@	"targets":[{"name":"unique_name.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_UNIQUENAME_HPP
#define TEXPAINTER_UNIQUENAME_HPP

#include <iterator>
#include <concepts>
#include <string>

namespace Texpainter
{
	namespace detail
	{
		template<class T, class Value>
		concept Container = requires(T a)
		{
			{
				*std::begin(a)
			}
			->std::same_as<std::add_lvalue_reference_t<std::add_const_t<Value>>>;
			std::end(a);
			{
				std::size(a)
			}
			->std::convertible_to<size_t>;
		};

		template<class T, class Value>
		concept Set = Container<T, Value>&& requires(T a, Value b)
		{
			{
				a.find(b)
			}
			->std::same_as<decltype(a.begin())>;
			{
				a.insert(b)
			}
			->std::same_as<std::pair<decltype(a.begin()), bool>>;
		};
	}

	template<detail::Set<std::string> StringSet>
	inline std::string generateEntryName(char const* name, StringSet& used_names)
	{
		std::string name_tmp{name};
		auto i = used_names.insert(name_tmp);
		if(!i.second)
		{
			auto k = 1;
			name_tmp += "_";
			while(true)
			{
				auto name_try = name_tmp + std::to_string(k);
				auto i = used_names.insert(name_try);
				if(i.second) { return name_try; }
				++k;
			}
		}
		return name_tmp;
	}
}

#endif