//@	{
//@	 "targets":[{"name":"start_dialog.hpp","type":"include"}]
//@ }

#ifndef TEXPAINTER_APP_STARTDIALOG_HPP
#define TEXPAINTER_APP_STARTDIALOG_HPP

#include "./ui/box.hpp"
#include "./ui/button.hpp"

namespace Texpainter::App
{
	class StartDialog
	{
	public:
		enum class ControlId : int
		{
			New,
			Open,
			ReadUsrGuide,
			Quit
		};

		explicit StartDialog(Ui::Container& owner)
		    : m_root{owner, Ui::Box::Orientation::Vertical}
		    , m_new{m_root, "Create a new document"}
		    , m_open{m_root, "Open a document"}
		    , m_read_usr_guide{m_root, "Read the quick-start guide"}
		    , m_quit{m_root, "Quit"}
		{
		}

		template<class EventHandler>
		void eventHandler(EventHandler& eh)
		{
			m_new.eventHandler<ControlId::New>(eh);
			m_open.eventHandler<ControlId::Open>(eh);
			m_read_usr_guide.eventHandler<ControlId::ReadUsrGuide>(eh);
			m_quit.eventHandler<ControlId::Quit>(eh);
		}

		template<ControlId id>
		void resetButtonState();

	private:
		Ui::Box m_root;
		Ui::Button m_new;
		Ui::Button m_open;
		Ui::Button m_read_usr_guide;
		Ui::Button m_quit;
	};

	template<>
	inline void StartDialog::resetButtonState<StartDialog::ControlId::New>()
	{
		m_new.state(false);
	}
}

#endif