//@	{
//@	"targets":[{"name":"keyboard_state.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_UI_KEYBOARDSTATE_HPP
#define TEXPAINTER_UI_KEYBOARDSTATE_HPP

#include "./scancode.hpp"

#include "utils/sorted_sequence.hpp"

#include <optional>

namespace Texpainter::Ui
{
	class KeyboardState
	{
	public:
		bool press(Scancode key) { return m_key_state.insert(std::make_pair(key, false)).second; }

		void release(Scancode key) { m_key_state.erase(key); }

		Scancode lastKey() const
		{
			auto keys = m_key_state.keysByIndex();
			if(std::begin(keys) == std::end(keys)) { return Scancode{0xff}; }
			auto keys_end = m_key_state.keysByIndex().end();
			--keys_end;
			return *keys_end;
		}

	private:
		SortedSequence<Ui::Scancode, bool> m_key_state;
	};
}

#endif