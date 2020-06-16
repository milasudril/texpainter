//@	{
//@  "targets":[{"name":"labeled_input.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_UI_LABELEDINPUT_HPP
#define TEXPAINTER_UI_LABELEDINPUT_HPP

#include "./box.hpp"
#include "./label.hpp"

namespace Texpainter::Ui
{
	template<class Widget>
	class LabeledInput
	{
	public:
		template<class... Args>
		explicit LabeledInput(Container& container,
		                      Box::Orientation orientation,
		                      const char* label,
		                      Args&&... args):
		   m_box{container, orientation},
		   m_label{m_box, label},
		   m_input_field{m_box, std::forward<Args>(args)...}
		{
			m_label.alignment(0.0f);
		}

		Label& label()
		{
			return m_label;
		}

		Label const& label() const
		{
			return m_label;
		}

		Widget& inputField()
		{
			return m_input_field;
		}

		Widget const& inputField() const
		{
			return m_input_field;
		}


	private:
		Box m_box;
		Label m_label;
		Widget m_input_field;
	};
}

#endif