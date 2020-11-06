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
		}
		catch(...)
		{
		}
	}

	void texpainterModelItemNameFromStringEmpty()
	{
		try
		{
			Texpainter::Model::ItemName test{std::string{""}};
			abort();
		}
		catch(...)
		{
		}
	}

	void texpainterModelItemNameFromCstrIllegalChar()
	{
		try
		{
			Texpainter::Model::ItemName test{"/"};
			abort();
		}
		catch(...)
		{
		}
	}

	void texpainterModelItemNameFromStringIllegalChar()
	{
		try
		{
			Texpainter::Model::ItemName test{std::string{"/"}};
			abort();
		}
		catch(...)
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