//@	{"targets":[{"name":"layer_creator.hpp","type":"include"}]}

#ifndef TEXPAINTER_LAYERCREATOR_HPP
#define TEXPAINTER_LAYERCREATOR_HPP

#include "model/layer.hpp"
#include "ui/box.hpp"
#include "ui/labeled_input.hpp"
#include "ui/text_entry.hpp"

namespace Texpainter
{
	class LayerCreator
	{
	public:
		explicit LayerCreator(Ui::Container& container, Size2d default_size):
		   m_root{container, Ui::Box::Orientation::Vertical},
		   m_name{m_root, Ui::Box::Orientation::Horizontal, "Name: "},
		   m_width{m_root, Ui::Box::Orientation::Horizontal, "Width: "},
		   m_height{m_root, Ui::Box::Orientation::Horizontal, "Height: "}
		{
			m_name.inputField().focus();
			m_width.inputField().content(std::to_string(default_size.width()).c_str());
			m_height.inputField().content(std::to_string(default_size.height()).c_str());
		}

		std::pair<std::string, Model::Layer> create() const
		{
			auto width = static_cast<uint32_t>(std::atoi(m_width.inputField().content()));
			auto height = static_cast<uint32_t>(std::atoi(m_height.inputField().content()));
			return {m_name.inputField().content(), Model::Layer{Size2d{width, height}}};
		}

	private:
		Ui::Box m_root;
		Ui::LabeledInput<Ui::TextEntry> m_name;
		Ui::LabeledInput<Ui::TextEntry> m_width;
		Ui::LabeledInput<Ui::TextEntry> m_height;
	};
}

#endif
