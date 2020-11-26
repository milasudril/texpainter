//@	{"targets":[{"name":"document_editor.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_DOCUMENTEDITOR_HPP
#define TEXPAINTER_APP_DOCUMENTEDITOR_HPP

// #include "./image_editor.hpp"
#include "./filter_graph_editor.hpp"

#include "model_new/document.hpp"

#include "ui/window.hpp"

namespace Texpainter::App
{
	template<class Widget>
	class WidgetWithWithWindow
	{
	public:
		template<class... Args>
		explicit WidgetWithWithWindow(char const* title, Args&&... args)
		    : m_window{title}
		    , m_widget{m_window, std::forward<Args>(args)...}
		{
			m_window.resize(Size2d{800, 500}).show();
		}

	private:
		Ui::Window m_window;
		Widget m_widget;
	};

	class DocumentEditor
	{
		using FilterGraphEditorWindow = WidgetWithWithWindow<FilterGraphEditor>;

	public:
		DocumentEditor(): m_document{Size2d{512, 512}}
		{
			m_filter = std::make_unique<FilterGraphEditorWindow>("Texpainter", m_document);
		}

	private:
		Model::Document m_document;
		std::unique_ptr<WidgetWithWithWindow<FilterGraphEditor>> m_filter;
	};
}

#endif