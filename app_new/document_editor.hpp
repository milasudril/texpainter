//@	{"targets":[{"name":"document_editor.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_DOCUMENTEDITOR_HPP
#define TEXPAINTER_APP_DOCUMENTEDITOR_HPP

// #include "./image_editor.hpp"
#include "./filter_graph_editor.hpp"

#include "model_new/document.hpp"

namespace Texpainter::App
{
	class DocumentEditor
	{
	public:
		DocumentEditor(): m_document{Size2d{512, 512}} {}

	private:
		Model::Document m_document;
		std::unique_ptr<FilterGraphEditor> m_filter;
	};
}

#endif