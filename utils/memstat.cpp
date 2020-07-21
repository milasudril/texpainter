//@	{
//@	 "targets":[{"name":"memstat.o", "type":"object"}]
//@	}

#include "./memstat.hpp"

#include <unistd.h>
#include <fcntl.h>

#include <string_view>
#include <cstdlib>
#include <array>

namespace
{
	size_t get_mem_avail(char* buffer, char* buffer_end)
	{
		enum class State : int
		{
			Key,
			Value,
			Skip
		};
		auto state_current = State::Key;
		auto pos_0         = buffer;
		while(buffer != buffer_end)
		{
			auto ch_in = *buffer;
			++buffer;
			switch(state_current)
			{
				case State::Key:
					switch(ch_in)
					{
						case ':':
							if(std::string_view{pos_0, static_cast<size_t>(buffer - pos_0)}
							   == "MemAvailable:")
							{
								state_current = State::Value;
								pos_0         = buffer;
							}
							else
							{
								state_current = State::Skip;
							}

							break;
					}
					break;

				case State::Value:
					switch(ch_in)
					{
						case '\n':
							*(buffer - 1) = '\0';  // To be able to use atoll
							return 1024 * static_cast<size_t>(atoll(pos_0));  // Reports are in kB
					}
					break;

				case State::Skip:
					switch(ch_in)
					{
						case '\n': state_current = State::Key; pos_0 = buffer;
					}
					break;
			}
		}
		return 0;
	}
}

size_t Texpainter::getAvailableRam()
{
	int const fd = open("/proc/meminfo", O_RDONLY);
	std::array<char, 4096> buffer{};
	auto n         = pread(fd, std::begin(buffer), std::size(buffer), 0);
	auto mem_avail = get_mem_avail(std::begin(buffer), std::begin(buffer) + n);
	close(fd);
	return mem_avail;
}