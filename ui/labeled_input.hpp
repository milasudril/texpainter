//@	{
//@  "targets":[{"name":"labeled_input.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_UI_LABELEDINPUT_HPP
#define TEXPAINTER_UI_LABELEDINPUT_HPP

#include "./box.hpp"
#include "./label.hpp"

namespace Texpainter::Ui
{
	template<class Widget, class LabelType = Label>
	class LabeledInput
	{
	public:
		template<class... Args>
		explicit LabeledInput(Container& container,
		                      Box::Orientation orientation,
		                      const char* label,
		                      Args&&... args)
		    : m_box{container, orientation}
		    , m_label{m_box, label}
		    , m_input_field{m_box.insertMode(Box::InsertMode{0, Box::Expand | Box::Fill}),
		                    std::forward<Args>(args)...}
		{
		}

		LabelType& label() { return m_label; }

		LabelType const& label() const { return m_label; }

		Widget& inputField() { return m_input_field; }

		Widget const& inputField() const { return m_input_field; }


	private:
		Box m_box;
		LabelType m_label;
		Widget m_input_field;
	};
}

#endif