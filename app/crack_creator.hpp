//@	{
//@	  "targets":[{"name":"crack_creator.hpp", "type":"include"}]
//@	 }

#ifndef TEXPAINTER_CRACKCREATOR_HPP
#define TEXPAINTER_CRACKCREATOR_HPP

#include "generators/crack_generator.hpp"
#include "ui/box.hpp"
#include "ui/image_view.hpp"
#include "ui/slider.hpp"
#include "ui/labeled_input.hpp"

#include <cmath>

namespace Texpainter
{
	class CrackCreator
	{
	public:
		enum class ControlId : int
		{
			NumberOfCracks,
			LineWidth,
			LineWidthGrowth,
			NoiseModulation,
			SegLength,
			BranchProb,
			MaxLength
		};

		explicit CrackCreator(Ui::Container& owner, Generators::CrackGenerator::Rng& rng):
		   m_preview{384, 384},
		   m_generator{rng},
		   m_root{owner, Ui::Box::Orientation::Vertical},
		   m_n_cracks{m_root, Ui::Box::Orientation::Horizontal, "Number of cracks: ", false},
		   m_line_width{m_root, Ui::Box::Orientation::Horizontal, "Initial width: ", false},
		   m_line_width_growth{m_root, Ui::Box::Orientation::Horizontal, "Width scaling factor: ", false},
		   m_noise_mod{m_root, Ui::Box::Orientation::Horizontal, "Noise modulation: ", false},
		   m_seg_length{m_root, Ui::Box::Orientation::Horizontal, "Line segment length: ", false},
		   m_branch_prob{m_root, Ui::Box::Orientation::Horizontal, "Branch probability: ", false},
		   m_max_length{m_root, Ui::Box::Orientation::Horizontal, "Max length: ", false},
		   m_img_view{m_root.insertMode(Ui::Box::InsertMode{0, Ui::Box::Expand | Ui::Box::Fill})}
		{
			m_img_view.minSize(Size2d{384, 384});
			m_preview = m_generator(Size2d{384, 384});
			m_img_view.image(m_preview);
			m_n_cracks.inputField().eventHandler<ControlId::NumberOfCracks>(*this).value(
			   (m_generator.nCracks() - 1.0) / 7.0);
			m_line_width.inputField().eventHandler<ControlId::LineWidth>(*this).value(
			   std::log2(m_generator.lineWidth()) / (10.0) + 1.0);
			m_line_width_growth.inputField().eventHandler<ControlId::LineWidthGrowth>(*this).value(
			   m_generator.lineWidthGrowth());
			m_noise_mod.inputField().eventHandler<ControlId::NoiseModulation>(*this).value(
			   m_generator.noiseMod());
			//		auto const val = static_cast<float>(std::exp2(-7.0 * (1.0 - slider.value()) - 1.0));

			m_seg_length.inputField().eventHandler<ControlId::SegLength>(*this).value(
			   std::log2((m_generator.segLength() + 1.0) / 7.0 + 1.0));
			m_branch_prob.inputField().eventHandler<ControlId::BranchProb>(*this).value(
			   m_generator.branchProb() / (0.5 - 1.0 / 256.0));
			m_max_length.inputField().eventHandler<ControlId::MaxLength>(*this).value(
			   m_generator.maxLength());
		}


		Model::Image generate(Size2d output_size)
		{
			return m_generator(output_size);
		}

		template<ControlId>
		void onChanged(Ui::Slider&);

	private:
		Model::Image m_preview;
		Generators::CrackGenerator m_generator;

		Ui::Box m_root;
		Ui::LabeledInput<Ui::Slider> m_n_cracks;
		Ui::LabeledInput<Ui::Slider> m_line_width;
		Ui::LabeledInput<Ui::Slider> m_line_width_growth;
		Ui::LabeledInput<Ui::Slider> m_noise_mod;
		Ui::LabeledInput<Ui::Slider> m_seg_length;
		Ui::LabeledInput<Ui::Slider> m_branch_prob;
		Ui::LabeledInput<Ui::Slider> m_max_length;

		Ui::ImageView m_img_view;
	};

	template<>
	void CrackCreator::onChanged<CrackCreator::ControlId::NumberOfCracks>(Ui::Slider& slider)
	{
		auto const val = static_cast<int>(1.0 + slider.value() * 7.0);
		m_preview = m_generator.nCracks(val)(m_preview.size());
		m_img_view.image(m_preview);
	}

	template<>
	void CrackCreator::onChanged<CrackCreator::ControlId::LineWidth>(Ui::Slider& slider)
	{
		auto const val = static_cast<float>(std::exp2(-10.0 * (1.0 - slider.value())));
		m_preview = m_generator.lineWidth(val)(m_preview.size());
		m_img_view.image(m_preview);
	}

	template<>
	void CrackCreator::onChanged<CrackCreator::ControlId::LineWidthGrowth>(Ui::Slider& slider)
	{
		auto const val = slider.value();
		m_preview = m_generator.lineWidthGrowth(val)(m_preview.size());
		m_img_view.image(m_preview);
	}

	template<>
	void CrackCreator::onChanged<CrackCreator::ControlId::NoiseModulation>(Ui::Slider& slider)
	{
		auto const val = slider.value();
		m_preview = m_generator.noiseMod(val)(m_preview.size());
		m_img_view.image(m_preview);
	}

	template<>
	void CrackCreator::onChanged<CrackCreator::ControlId::SegLength>(Ui::Slider& slider)
	{
		auto const val = static_cast<float>(std::exp2(-7.0 * (1.0 - slider.value()) - 1.0));
		m_preview = m_generator.segLength(val)(m_preview.size());
		m_img_view.image(m_preview);
	}

	template<>
	void CrackCreator::onChanged<CrackCreator::ControlId::BranchProb>(Ui::Slider& slider)
	{
		auto const val = (0.5 - 1.0 / 256.0) * slider.value();
		m_preview = m_generator.branchProb(val)(m_preview.size());
		m_img_view.image(m_preview);
	}

	template<>
	void CrackCreator::onChanged<CrackCreator::ControlId::MaxLength>(Ui::Slider& slider)
	{
		auto const val = slider.value();
		m_preview = m_generator.maxLength(val)(m_preview.size());
		m_img_view.image(m_preview);
	}
}

#endif