//@	{
//@	 "targets":[{"name":"mallochook.o", "type":"object", "dependencies":[{"ref":"dl", "rel":"external"}]}]
//@	}

#include "./mallochook.hpp"

#include <dlfcn.h>

#include <map>
#include <cassert>

namespace
{
	std::map<void const*, size_t> s_blocks;
	bool s_armed = false;
}

size_t Testutils::MallocHook::blockSize(void const* ptr)
{
	auto i = s_blocks.find(ptr);
	assert(i != std::end(s_blocks));
	return i->second;
}

void Testutils::MallocHook::init()
{
	s_blocks.clear();
	s_armed = true;
}

size_t Testutils::MallocHook::size() { return s_blocks.size(); }

void Testutils::MallocHook::disarm() { s_armed = false; }

using MallocFunc = void* (*)(size_t);
using FreeFunc   = void (*)(void*);

extern "C"
{
	void* malloc(size_t size) noexcept
	{
		auto real_malloc = reinterpret_cast<MallocFunc>(dlsym(RTLD_NEXT, "malloc"));
		auto ret         = real_malloc(size);
		assert(ret != nullptr);

		if(s_armed)
		{
			s_armed = false;
			s_blocks.insert(std::make_pair(ret, size));
			s_armed = true;
		}

		return ret;
	}

	void free(void* ptr) noexcept
	{
		auto real_free = reinterpret_cast<FreeFunc>(dlsym(RTLD_NEXT, "free"));
		real_free(ptr);

		if(s_armed)
		{
			s_armed = false;
			s_blocks.erase(ptr);
			s_armed = true;
		}
	}
}