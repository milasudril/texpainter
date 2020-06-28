//@	{"targets":[{"name":"layerstack_control.hpp", "type":"include"}]}

#ifndef TEXPAINTER_LAYERSTACKCONTROL_HPP
#define TEXPAINTER_LAYERSTACKCONTROL_HPP

#include "model/layer_stack.hpp"
#include "ui/box.hpp"

namespace Texpainter
{
	class LayerStackControl
	{
		public:
			explicit LayerStackControl(Ui::Container& owner):
				m_root{owner, Ui::Box::Orientation::Horizontal},
				m_layer_selector{m_root},
				m_layer_new{m_root, "+"},
				m_layer_delete{m_root, "-"},
				m_layer_move_up{m_root, "↑"},
				m_layer_move_down{m_root, "↓"}
			{}

			LayerStackControl& layers(Model::LayerStack&& l, std::vector<std::string>&& n)
			{
				m_layers = std::move(l);
				m_layer_names = std::move(n);
				m_layer_selector.clear();
				std::ranges::for_each(m_layer_names, [&layer_selector = m_layer_selector](auto const& item) {
					layer_selector.append(item.c_str());
				});
				return *this;
			}

		private:
			Model::LayerStack m_layers;
			std::vector<std::string> m_layer_names;

			Ui::Box m_root;
			Ui::Combobox m_layer_selector;
			Ui::Button m_layer_new;
			Ui::Button m_layer_delete;
			Ui::Button m_layer_move_up;
			Ui::Button m_layer_move_down;
	};
}

#endif