
//@	{
//@	 "targets":[{"name":"signaling_counter.hpp","type":"include"
//@		, "dependencies":[{"ref":"pthread", "rel":"external"}]}]
//@	}

#ifndef TEXPAINTER_SCHED_SIGNALINGCOUNTER_HPP
#define TEXPAINTER_SCHED_SIGNALINGCOUNTER_HPP

#include <condition_variable>
#include <mutex>

namespace Texpainter::Sched
{
	template<class T>
	class SignalingCounter
	{
	public:
		explicit SignalingCounter(): m_value{0} {}

		void waitAndReset(T value)
		{
			std::unique_lock lock{m_mtx};
			m_cv.wait(lock, [this, value]() { return m_value == value; });
			m_value = 0;
		}

		SignalingCounter& operator++()
		{
			std::lock_guard lock{m_mtx};
			++m_value;
			m_cv.notify_all();
			return *this;
		}

		bool operator==(T value) const
		{
			std::lock_guard lock{m_mtx};
			return m_value == value;
		}

		bool operator!=(T value) const { return !(*this == value); }

	private:
		mutable std::mutex m_mtx;
		mutable std::condition_variable m_cv;
		T m_value;
	};
}

#endif