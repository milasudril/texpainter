//@	{
//@	  "targets":[{"name":"event.hpp", "type":"include"}]
//@	}

#ifndef TEXPAINTER_SCHED_EVENT_HPP
#define TEXPAINTER_SCHED_EVENT_HPP

#include <mutex>

namespace Texpainter::Sched
{
	class Event
	{
	public:
		Event(): m_status{false} {}

		void waitAndReset()
		{
			std::unique_lock lock{m_mtx};
			m_cv.wait(lock, [&status = m_status]() { return status; });
			m_status = false;
		}

		void set()
		{
			std::lock_guard lock{m_mtx};
			m_status = true;
			m_cv.notify_one();
		}

	private:
		std::mutex m_mtx;
		bool m_status;
		std::condition_variable m_cv;
	};
}

#endif