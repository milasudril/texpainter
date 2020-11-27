//@	{"targets":[{"name":"image_editor.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_IMAGEEDITOR_HPP
#define TEXPAINTER_APP_IMAGEEDITOR_HPP

#include "model_new/document.hpp"
#include "ui/box.hpp"
#include "ui/image_view.hpp"
#include "ui/palette_view.hpp"
#include "ui/listbox.hpp"

namespace Texpainter::App
{
	class ImageEditor
	{
	public:
		explicit ImageEditor(Ui::Container& owner, Model::Document& doc)
		    : m_doc{doc}
		    , m_root{owner, Ui::Box::Orientation::Vertical}
		    , m_selectors{m_root, Ui::Box::Orientation::Horizontal}
		    , m_img_view{m_root}
		{
		}

	private:
		std::reference_wrapper<Model::Document> m_doc;
		Ui::Box m_root;
		Ui::Box m_selectors;
		Ui::ImageView m_img_view;
	};
}

#endif