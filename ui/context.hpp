//@	{
//@	 "targets":[{"name":"context.hpp", "type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"context.o", "rel":"implementation"}]
//@	 }

#ifndef TEXPAINTER_UI_CONTEXT_HPP
#define TEXPAINTER_UI_CONTEXT_HPP

#include "./keyboard_state.hpp"

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

		KeyboardState const& keyboardState() const { return m_key_state; }

	private:
		Context();
		KeyboardState m_key_state;
	};
}

#endif