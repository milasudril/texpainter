//@	{
//@	"targets":[{"name":"item_name.test","type":"application", "autorun":1}]
//@	}

#include "./item_name.hpp"

#include <cassert>

namespace Testcases
{
	void texpainterModelItemNameFromCstr()
	{
		Texpainter::Model::ItemName test{"foobar"};

		assert(test.value() == "foobar");
		assert(strcmp(test.c_str(), "foobar") == 0);
	}

	void texpainterModelItemNameFromString()
	{
		Texpainter::Model::ItemName test{std::string{"foobar"}};

		assert(test.value() == "foobar");
		assert(strcmp(test.c_str(), "foobar") == 0);
	}

	void texpainterModelItemNameFromCstrEmpty()
	{
		try
		{
			Texpainter::Model::ItemName test{""};
			abort();
		} catch(...)
		{
		}
	}

	void texpainterModelItemNameFromStringEmpty()
	{
		try
		{
			Texpainter::Model::ItemName test{std::string{""}};
			abort();
		} catch(...)
		{
		}
	}

	void texpainterModelItemNameFromCstrIllegalChar()
	{
		try
		{
			Texpainter::Model::ItemName test{"/"};
			abort();
		} catch(...)
		{
		}
	}

	void texpainterModelItemNameFromStringIllegalChar()
	{
		try
		{
			Texpainter::Model::ItemName test{std::string{"/"}};
			abort();
		} catch(...)
		{
		}
	}

	void texpainterModelItemNameFromFilename1()
	{
		auto test = Texpainter::Model::createItemNameFromFilename("/foobar/kaka.exr");
		assert(test.value() == "kaka");
	}

	void texpainterModelItemNameFromFilename2()
	{
		auto test = Texpainter::Model::createItemNameFromFilename("/foobar/kaka");
		assert(test.value() == "kaka");
	}

	void texpainterModelItemNameFromFilename3()
	{
		auto test = Texpainter::Model::createItemNameFromFilename("kaka.exr");
		assert(test.value() == "kaka");
	}

	void texpainterModelItemNameFromFilename4()
	{
		auto test = Texpainter::Model::createItemNameFromFilename("/foobar/.kaka");
		assert(test.value() == ".kaka");
	}

	void texpainterModelItemNameFromFilename5()
	{
		auto test = Texpainter::Model::createItemNameFromFilename("kaka.foo.exr");
		assert(test.value() == "kaka.foo");
	}
}

int main()
{
	Testcases::texpainterModelItemNameFromCstr();
	Testcases::texpainterModelItemNameFromString();
	Testcases::texpainterModelItemNameFromCstrEmpty();
	Testcases::texpainterModelItemNameFromStringEmpty();
	Testcases::texpainterModelItemNameFromCstrIllegalChar();
	Testcases::texpainterModelItemNameFromStringIllegalChar();
	Testcases::texpainterModelItemNameFromFilename1();
	Testcases::texpainterModelItemNameFromFilename2();
	Testcases::texpainterModelItemNameFromFilename3();
	Testcases::texpainterModelItemNameFromFilename4();
	Testcases::texpainterModelItemNameFromFilename5();

	return 0;
}


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

			if(std::any_of(c_str, c_str + l, [](auto val) { val == '/'; }))
			{ throw std::string{"The name contains at least one illegal character."}; }

			m_value = c_str;
		}

		explicit ItemName(std::string&& name): m_value{std::move(name)}
		{
			if(m_value.size() == 0) { throw "Empty names are not allowed."; }

			if(std::ranges::any_of(std::begin(m_value), std::end(m_value), [](auto val) {
				   return val == 0 || val == '/';
			   }))
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

	inline ItemName createItemNameFromFilename(char const* filename)
	{
		auto range = std::span{filename, strlen(filename)};

		// Assume UNIX and not DOS
		auto name_begin = std::find(std::rbegin(range), std::rend(range), '/').base();
		auto name_end   = std::find(std::rbegin(range), std::rend(range), '.').base() - 1;

		if(name_begin >= name_end) { return ItemName{std::string{name_begin, std::end(range)}}; }

		return ItemName{std::string{name_begin, name_end}};
	}
}

#endif