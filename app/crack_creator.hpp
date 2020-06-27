//@	{
//@	  "targets":[{"name":"crack_creator.hpp", "type":"include"}]
//@	 }

#include "generators/crack_generator.hpp"
#include "ui/image_view.hpp"
#include "ui/box.hpp"

#ifndef TEXPAINTER_CRACKCREATOR_HPP
#define TEXPAINTER_CRACKCREATOR_HPP

namespace Texpainter
{
	class CrackCreator
	{
	public:
		explicit CrackCreator(Ui::Container& owner, Generators::CrackGenerator::Rng& rng):
		   m_preview{384, 384},
		   m_generator{rng},
		   m_root{owner, Ui::Box::Orientation::Vertical},
		   m_img_view{m_root}
		{
			m_img_view.minSize(Size2d{384, 384});
			m_preview = m_generator(Size2d{384, 384});
			m_img_view.image(m_preview);
		}


		Model::Image generate(Size2d output_size)
		{
			return m_generator(output_size);
		}

	private:
		Model::Image m_preview;
		Generators::CrackGenerator m_generator;

		Ui::Box m_root;
		Ui::ImageView m_img_view;
	};
}

#endif