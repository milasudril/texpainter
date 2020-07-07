//@	{"targets":[{"name":"layer_creator.hpp","type":"include"}]}

#ifndef TEXPAINTER_LAYERCREATOR_HPP
#define TEXPAINTER_LAYERCREATOR_HPP

#include "model/layer.hpp"
#include "ui/box.hpp"
#include "ui/labeled_input.hpp"
#include "ui/text_entry.hpp"

#include <stdexcept>

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

		explicit LayerCreator(Ui::Container& container, Size2d default_size, Size2d max_size):
		   m_root{container, Ui::Box::Orientation::Vertical},
		   m_name{m_root, Ui::Box::Orientation::Horizontal, "Name: "},
		   m_width{m_root, Ui::Box::Orientation::Horizontal, "Width: "},
		   m_height{m_root, Ui::Box::Orientation::Horizontal, "Height: "},
		   m_width_str{std::to_string(default_size.width())},
		   m_height_str{std::to_string(default_size.height())},
		   m_max_size{max_size}
		{
			m_name.inputField().focus();
			m_width.inputField()
			   .content(m_width_str.c_str())
			   .alignment(1.0f)
			   .width(5)
			   .eventHandler<ControlId::Width>(*this);
			m_height.inputField()
			   .content(m_width_str.c_str())
			   .alignment(1.0f)
			   .width(5)
			   .eventHandler<ControlId::Height>(*this);
		}

		std::pair<std::string, Model::Layer> create() const
		{
			auto const width = static_cast<uint32_t>(std::atoi(m_width.inputField().content()));
			auto const height = static_cast<uint32_t>(std::atoi(m_height.inputField().content()));

			auto size = Size2d{width, height};
			if(!isSupported<Model::Pixel>(size))
			{
				throw std::runtime_error{
				   "A layer of this size cannot be created. The number of bytes required to create a layer of "
				   "this size exeeds the largest supported integer value."};
			}
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
		Size2d m_max_size;

		char const* make_valid_int(char const* str, uint32_t max_size, std::string& replace_str)
		{
			char* end;
			errno = 0;
			auto val = strtol(str, &end, 10);
			if(errno != 0 || *end != '\0' || val <= 0 || val > std::numeric_limits<uint32_t>::max())
			{ return replace_str.c_str(); }

			val = std::min(static_cast<uint32_t>(val), max_size);
			replace_str = std::to_string(val);
			return replace_str.c_str();
		}
	};

	template<>
	inline void LayerCreator::onChanged<LayerCreator::ControlId::Width>(Ui::TextEntry& entry)
	{
		entry.content(make_valid_int(entry.content(), m_max_size.width(), m_width_str));
	}

	template<>
	inline void LayerCreator::onChanged<LayerCreator::ControlId::Height>(Ui::TextEntry& entry)
	{
		entry.content(make_valid_int(entry.content(), m_max_size.height(), m_height_str));
	}
}

#endif
