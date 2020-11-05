//@	{
//@	"targets":[{"name":"item_name.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_MODEL_ITEMNAME_HPP
#define TEXPAINTER_MODEL_ITEMNAME_HPP

#include <string>
#include <algorithm>
#include <cstring>
#include <compare>

namespace Texpainter::Model
{
	class ItemName
	{
	public:
		ItemName() = default;

		explicit ItemName(char const* c_str)
		{
			auto l = strlen(c_str);

			if(l == 0) { throw "Empty names are not allowed."; }

			if(std::any_of(c_str, c_str + l, [](auto val) { return val == 0; }))
			{ throw std::string{"The name contains at least one illegal character."}; }

			m_value = c_str;
		}

		explicit ItemName(std::string&& name):m_value{std::move(name)}
		{
			if(m_value.size() == 0) { throw "Empty names are not allowed."; }

			if(std::ranges::any_of(std::begin(m_value), std::end(m_value), [](auto val) { return val == 0; }))
			{ throw std::string{"The name contains at least one illegal character."}; }
		}

		std::string const& value() const { return m_value; }

		bool valid() const { return !m_value.empty(); }

		char const* c_str() const { return m_value.c_str(); }

	private:
		std::string m_value;
	};

	inline std::string const& toString(ItemName const& item) { return item.value(); }
	std::string const& toString(ItemName const&& item) = delete;

	inline bool operator<(ItemName const& a, ItemName const& b) { return a.value() < b.value(); }
}

#endif