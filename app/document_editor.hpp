//@	{"targets":[{"name":"document_editor.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_DOCUMENTEDITOR_HPP
#define TEXPAINTER_APP_DOCUMENTEDITOR_HPP

#include "./main_menu.hpp"

#include "ui/window.hpp"
#include "ui/box.hpp"
#include "ui/menu_builder.hpp"

namespace Texpainter::App
{
	template<class Widget>
	class DocumentEditor
	{
	public:
		DocumentEditor(DocumentEditor&&) = delete;

		template<class... Args>
		explicit DocumentEditor(char const* title, Args&&... args)
		    : m_window{title}
		    , m_root{m_window, Ui::Box::Orientation::Vertical}
		    , m_menu{m_root}
		    , m_widget{m_root.insertMode(Ui::Box::InsertMode{0, Ui::Box::Fill | Ui::Box::Expand}),
		               std::forward<Args>(args)...}
		{
			m_window.resize(Size2d{800, 500}).show();

			m_menu.eventHandler(*this);
		}

		auto& window() { return m_window; }

		auto& widget() { return m_widget; }

		auto& menu() { return m_menu; }

		template<auto id>
		void onActivated(Ui::MenuItem&)
		{
			throw "Unimplemented";
		}

		template<auto id>
		void handleException(char const* msg, Ui::MenuItem const& item)
		{
			fprintf(stderr, "Failed to %s: %s\n", item.label(), msg);
		}

	private:
		Ui::Window m_window;
		Ui::Box m_root;
		Ui::MenuBuilder<MainMenuItem, MainMenuItemTraits> m_menu;
		Widget m_widget;
	};
}

#endif