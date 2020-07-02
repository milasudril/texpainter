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
		enum class ControlId : int
		{
			Width,
			Height
		};

		explicit LayerCreator(Ui::Container& container, Size2d default_size):
		   m_root{container, Ui::Box::Orientation::Vertical},
		   m_name{m_root, Ui::Box::Orientation::Horizontal, "Name: "},
		   m_width{m_root, Ui::Box::Orientation::Horizontal, "Width: "},
		   m_height{m_root, Ui::Box::Orientation::Horizontal, "Height: "},
		   m_width_str{std::to_string(default_size.width())},
		   m_height_str{std::to_string(default_size.height())}
		{
			m_name.inputField().focus();
			m_width.inputField()
			   .content(m_width_str.c_str())
			   .alignment(1.0f)
			   .eventHandler<ControlId::Width>(*this);
			m_height.inputField()
			   .content(m_width_str.c_str())
			   .alignment(1.0f)
			   .eventHandler<ControlId::Height>(*this);
		}

		std::pair<std::string, Model::Layer> create() const
		{
			auto width = static_cast<uint32_t>(std::atoi(m_width.inputField().content()));
			auto height = static_cast<uint32_t>(std::atoi(m_height.inputField().content()));
			return {m_name.inputField().content(), Model::Layer{Size2d{width, height}}};
		}

		template<ControlId>
		void onChanged(Ui::TextEntry& entry);

	private:
		Ui::Box m_root;
		Ui::LabeledInput<Ui::TextEntry> m_name;
		Ui::LabeledInput<Ui::TextEntry> m_width;
		Ui::LabeledInput<Ui::TextEntry> m_height;

		std::string m_width_str;
		std::string m_height_str;

		char const* make_valid_int(char const* str, std::string& replace_str)
		{
			char* end;
			errno = 0;
			auto val = strtol(str, &end, 10);
			if(errno != 0 || *end != '\0' || val <= 0 || val > std::numeric_limits<uint32_t>::max())
			{ return replace_str.c_str(); }
			replace_str = str;
			return str;
		}
	};

	template<>
	void LayerCreator::onChanged<LayerCreator::ControlId::Width>(Ui::TextEntry& entry)
	{
		entry.content(make_valid_int(entry.content(), m_width_str));
	}

	template<>
	void LayerCreator::onChanged<LayerCreator::ControlId::Height>(Ui::TextEntry& entry)
	{
		entry.content(make_valid_int(entry.content(), m_height_str));
	}
}

#endif
