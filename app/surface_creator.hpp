//@	{
//@	  "targets":[{"name":"surface_creator.hpp", "type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"surface_creator.o","rel":"implementation"}]
//@	 }

#include "generators/surface_generator.hpp"
#include "ui/labeled_input.hpp"
#include "ui/combobox.hpp"
#include "ui/slider.hpp"
#include "ui/image_view.hpp"
#include "ui/box.hpp"
#include "ui/button.hpp"

#ifndef TEXPAINTER_SURFACECREATOR_HPP
#define TEXPAINTER_SURFACECREATOR_HPP

namespace Texpainter
{
	class SurfaceCreator
	{
	public:
		enum class ControlId : int
		{
			FilterGraph,
			Orientation,
			HorzCutoff,
			VertCutoff,
			LockHvCutoff
		};

		explicit SurfaceCreator(Ui::Container& owner);

		template<ControlId id>
		void onChanged(Ui::Combobox&);

		template<ControlId id>
		void onChanged(Ui::Slider&);

		template<ControlId id>
		void onClicked(Ui::Button&);

	private:
		Model::Image m_preview;
		Generators::SurfaceGenerator m_generator;

		Ui::Box m_root;
		Ui::LabeledInput<Ui::Combobox> m_filter_graph;
		Ui::LabeledInput<Ui::Slider> m_orientation;
		Ui::Box m_cutoff;
		Ui::Box m_cutoff_sliders;
		Ui::LabeledInput<Ui::Slider> m_horz_cutoff;
		Ui::LabeledInput<Ui::Slider> m_vert_cutoff;
		Ui::Button m_cutoff_sliders_lock;
		Ui::ImageView m_img_view;
	};
}

#endif