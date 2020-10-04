//@	{"targets":[{"name":"error_message_dialog.hpp","type":"include"}]}

#ifndef TEXPAINTER_UI_ERRORMESSAGEDIALOG_HPP
#define TEXPAINTER_UI_ERRORMESSAGEDIALOG_HPP

#include "./dialog.hpp"
#include "./label.hpp"

namespace Texpainter::Ui
{
	class ErrorMessageDialog
	{
		using MsgBox = Dialog<Label, DialogOk>;

	public:
		void show(Container& owner, char const* title, char const* message)
		{
			m_dialog = std::make_unique<MsgBox>(owner, title, message);
			m_dialog->template eventHandler<0>(*this);
		}

		template<int>
		void confirmPositive(MsgBox&) noexcept
		{
			m_dialog.reset();
		}

		template<int>
		void handleException(char const* message, MsgBox&)
		{
			fprintf(stderr, "An error occured during processing of an error: %s\n", message);
			std::terminate();
		}

	private:
		std::unique_ptr<MsgBox> m_dialog;
	};

}

#endif