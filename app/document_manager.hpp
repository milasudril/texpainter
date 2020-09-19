//@	{
//@	"targets":[{"name":"document_manager.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_APP_DOCUMENTMANAGER_HPP
#define TEXPAINTER_APP_DOCUMENTMANAGER_HPP

#include "model/document.hpp"

namespace Texpainter
{
	class DocumentManager
	{
	public:
		Model::Document* createDocument(Size2d canvas_size)
		{
			m_current_document = std::make_unique<Model::Document>(canvas_size);
			return currentDocument();
		}

		//NOTE: This is const, because it does not change the statue of manager
		Model::Document* currentDocument() const { return m_current_document.get(); }

	private:
		std::unique_ptr<Model::Document> m_current_document;
	};
}

#endif