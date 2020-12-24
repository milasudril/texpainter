//@	{
//@	 "targets":[{"name":"context.hpp", "type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"context.o", "rel":"implementation"}]
//@	 }

#ifndef TEXPAINTER_UI_CONTEXT_HPP
#define TEXPAINTER_UI_CONTEXT_HPP

#include "./keyboard_state.hpp"

#include "utils/unique_function.hpp"

#include <queue>

namespace Texpainter::Ui
{
	class Context
	{
	public:
		static Context& get()
		{
			static Context ctxt;
			return ctxt;
		}

		Context(const Context&) = delete;
		Context& operator=(const Context&) = delete;
		Context(Context&&)                 = delete;
		Context& operator=(Context&&) = delete;

		void run();

		void exit();

		KeyboardState const& keyboardState() const { return m_key_state; }

		template<class F, class... Args>
		void scheduleAction(uint64_t iteration_offset, F&& func, Args&&... args)
		{
			m_actions.push(
			    std::pair{m_event_index + iteration_offset,
			              std::bind_front(std::forward<F>(func), std::forward<Args>(args)...)});
		}


	private:
		Context();
		bool m_stop;
		uint64_t m_event_index;

		using ScheduledAction = std::pair<uint64_t, UniqueFunction<void()>>;

		struct Compare
		{
			bool operator()(ScheduledAction const& a, ScheduledAction const& b) const
			{
				return a.first < b.first;
			}
		};

		std::priority_queue<ScheduledAction, std::vector<ScheduledAction>, Compare> m_actions;
		KeyboardState m_key_state;
	};
}

#endif