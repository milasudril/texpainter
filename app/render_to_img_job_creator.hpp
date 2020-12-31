//@	{"targets":[{"name":"render_to_img_job_creator.hpp", "type":"include"}]}

#ifndef TEXPAINTER_APP_RENDERTOIMGJOBCREATOR_HPP
#define TEXPAINTER_APP_RENDERTOIMGJOBCREATOR_HPP

#include "./render_options_input.hpp"

#include "model/item_name.hpp"
#include "ui/text_entry.hpp"

namespace Texpainter::App
{
	class RenderToImgJobCreator
	{
		enum class ControlId : int
		{
			ItemName
		};

	public:
		struct Options
		{
			Model::ItemName name;
			uint32_t supersampling;
		};

		explicit RenderToImgJobCreator(Ui::Container& container)
		    : m_root{container, Ui::Box::Orientation::Vertical}
		    , m_item_name{m_root, Ui::Box::Orientation::Horizontal, "Name: "}
		    , m_render_opts{m_root, Ui::Box::Orientation::Vertical, "Render options:"}
		{
		}

		Options value() const
		{
			return Options{Model::ItemName{m_item_name.inputField().content()},
			               m_render_opts.inputField().value()};
		}

	private:
		Ui::Box m_root;
		Ui::LabeledInput<Ui::TextEntry> m_item_name;
		Ui::LabeledInput<RenderOptionsInput> m_render_opts;
	};
}

#endif