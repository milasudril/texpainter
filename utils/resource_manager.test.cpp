//@	{"targets":[{"name":"resource_manager.test.cpp", "type":"application", "autorun":1}]}

#include "./resource_manager.hpp"

#include <cassert>

namespace Testcases
{
	void texpainterResourceManagerCreateAndGetResource()
	{
		Texpainter::ResourceManager<int> resources;
		auto result = resources.create(1234);
		assert(result->second.get() == 1234);
		assert(result->second.inUse());

		assert(resources.hasResource(result->first));
		auto item = resources.get(result->first);
		assert(item.get() == result->second.get());
		assert(item.inUse());

		assert(resources.objectCount() == 1);

		auto items = resources.objects();
		assert(items.size() == 1);
		auto first = std::begin(items);
		assert(first->first == result->first);
		assert(first->second.get() == result->second.get());
		assert(first->second.inUse() == result->second.inUse());
	}


	void texpainterResourceManagerRemoveResource()
	{
		Texpainter::ResourceManager<int> resources;
		auto result = resources.create(1234);
		assert(resources.hasResource(result->first));

		auto result_2 = resources.incUsecount(result->first);
		assert(result_2->first == result->first);
		assert(result_2->second.get() == result->second.get());

		assert(result_2 == result);

		result_2->second.get() = 456;
		assert(result_2->second.get() == result->second.get());
	}

}

int main()
{
	Testcases::texpainterResourceManagerCreateAndGetResource();
	Testcases::texpainterResourceManagerRemoveResource();
}