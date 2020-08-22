//@	{"targets":[{"name":"layer_creator.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_LAYERCREATOR_HPP
#define TEXPAINTER_APP_LAYERCREATOR_HPP

#include "./size_input.hpp"

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

		explicit LayerCreator(Ui::Container& container, Size2d default_size, Size2d max_size)
		    : m_root{container, Ui::Box::Orientation::Vertical}
		    , m_name{m_root, Ui::Box::Orientation::Horizontal, "Name: "}
		    , m_size{m_root, default_size, max_size}
		{
			m_name.inputField().focus();
		}

		struct LayerInfo
		{
			std::string name;
			Size2d size;
		};

		auto value() const { return LayerInfo{m_name.inputField().content(), m_size.value()}; }

	private:
		Ui::Box m_root;
		Ui::LabeledInput<Ui::TextEntry> m_name;
		SizeInput m_size;
	};
}

#endif