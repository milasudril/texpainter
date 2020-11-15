//@	{
//@	  "targets":[{"name":"thread_count.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_SCHED_THREADCOUNT_HPP
#define TEXPAINTER_SCHED_THREADCOUNT_HPP

#include <sys/sysinfo.h>

#include <algorithm>

namespace Texpainter::Sched
{
	class ThreadCount
	{
	public:
		ThreadCount(): m_val{-1} {}

		int value() const { return m_val < 0 ? std::max(get_nprocs(), 1) : m_val; }

	private:
		int m_val;
	};
}

#endif