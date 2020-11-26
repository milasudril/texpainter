//@	{"targets":[{"name":"document_editor.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_DOCUMENTEDITOR_HPP
#define TEXPAINTER_APP_DOCUMENTEDITOR_HPP

// #include "./image_editor.hpp"
#include "./filter_graph_editor.hpp"

#include "model_new/document.hpp"

#include "ui/window.hpp"
#include "ui/image_view.hpp"

namespace Texpainter::App
{
	template<class Widget>
	class WidgetWithWindow
	{
	public:
		template<class... Args>
		explicit WidgetWithWindow(char const* title, Args&&... args)
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
		using FilterGraphEditorWindow = WidgetWithWindow<FilterGraphEditor>;
		using OutputWindow = WidgetWithWindow<Ui::ImageView>;

	public:
		DocumentEditor(): m_document{Size2d{512, 512}}
		{
			m_filter = std::make_unique<FilterGraphEditorWindow>("Texpainter", m_document);
			m_output_window = std::make_unique<OutputWindow>("Texpainter");
		}

	private:
		Model::Document m_document;
		std::unique_ptr<FilterGraphEditorWindow> m_filter;
		std::unique_ptr<OutputWindow> m_output_window;
	};
}

#endif