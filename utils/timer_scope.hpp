//@	{"targets":[{"name":"timer_scope.hpp", "type":"include"}]}

#ifndef TEXPAINTER_UTILS_TIMERSCOPE_HPP
#define TEXPAINTER_UTILS_TIMERSCOPE_HPP

#include <cstdio>
#include <chrono>

namespace Texpainter
{
	class TimerScope
	{
	public:
		[[nodiscard]] explicit TimerScope(char const* file, char const* function, size_t line)
		    : m_start{std::chrono::steady_clock::now()}
		    , r_file{file}
		    , r_func{function}
		    , m_line{line}
		{
			printf("{");
		}

		~TimerScope()
		{
			auto const dt = std::chrono::steady_clock::now() - m_start;
			printf("\"%s:%s:%zu %ld\";}\n", r_file, r_func, m_line, dt.count());
		}

	private:
		std::chrono::steady_clock::time_point m_start;
		char const* r_file;
		char const* r_func;
		size_t m_line;
	};
}

#endif