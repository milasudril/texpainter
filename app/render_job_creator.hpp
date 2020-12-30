//@	{"targets":[{"name":"render_job_creator.hpp", "type":"include"}]}

#ifndef TEXPAINTER_APP_RENDERJOBCREATOR_HPP
#define TEXPAINTER_APP_RENDERJOBCREATOR_HPP

#include "ui/box.hpp"
#include "ui/slider.hpp"
#include "ui/labeled_input.hpp"

namespace Texpainter::App
{
	class RenderJobCreator
	{
	public:
		explicit RenderJobCreator(Ui::Container& container)
		    : m_root{container, Ui::Box::Orientation::Vertical}
		    , m_supersampling{m_root, Ui::Box::Orientation::Vertical, "Supersampling:", false}
		{
		}

	private:
		Ui::Box m_root;
		Ui::LabeledInput<Ui::Slider> m_supersampling;
	};
}

#endif