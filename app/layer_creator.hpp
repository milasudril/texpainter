//@	{"targets":[{"name":"layer_creator.hpp","type":"include"}]}

#ifndef TEXPAINTER_LAYERCREATOR_HPP
#define TEXPAINTER_LAYERCREATOR_HPP

#include "./size_input.hpp"

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

		explicit LayerCreator(Ui::Container& container, Size2d default_size, Size2d max_size)
		    : m_root{container, Ui::Box::Orientation::Vertical}
		    , m_name{m_root, Ui::Box::Orientation::Horizontal, "Name: "}
		    , m_size{m_root, default_size, max_size}
		{
			m_name.inputField().focus();
		}

		std::pair<std::string, Model::Layer> create() const
		{
			auto size = m_size.value();
			if(!isSupported<Model::Pixel>(size))
			{
				throw std::runtime_error{"A layer of this size cannot be created. The number of "
				                         "bytes required to create a layer of "
				                         "this size exeeds the largest supported integer value."};
			}
			return {m_name.inputField().content(), Model::Layer{size}};
		}

		template<ControlId>
		void onChanged(Ui::TextEntry& entry);

	private:
		Ui::Box m_root;
		Ui::LabeledInput<Ui::TextEntry> m_name;
		SizeInput m_size;
	};
}

#endif