//@	{"targets":[{"name":"layerstack_control.hpp", "type":"include"}]}

#ifndef TEXPAINTER_LAYERSTACKCONTROL_HPP
#define TEXPAINTER_LAYERSTACKCONTROL_HPP

#include "./create_layer_dlg.hpp"
#include "model/layer_stack.hpp"
#include "ui/box.hpp"
#include "ui/separator.hpp"

namespace Texpainter
{
	class LayerStackControl
	{
		using CreateLayerDlg = Ui::Dialog<LayerCreator>;

	public:
		enum class ControlId : int
		{
			LayerNew
		};

		explicit LayerStackControl(Ui::Container& owner):
		   m_root{owner, Ui::Box::Orientation::Horizontal},
		   m_layer_selector{m_root},
		   m_layer_new{m_root, "＋"},
		   m_separator_0{m_root},
		   m_layer_copy{m_root, "Copy"},
		   m_layer_link{m_root, "Link"},
		   m_layer_delete{m_root, "−"},
		   m_separator_1{m_root},
		   m_layer_move_up{m_root, "↑"},
		   m_layer_move_down{m_root, "↓"},
		   m_separator_2{m_root},
		   m_layer_solo{m_root, "Solo"},
		   m_layer_mute{m_root, "Hide"},
		   m_separator_3{m_root},
		   m_blend_func{m_root, "f(x)"}
		{
			m_layer_new.eventHandler<ControlId::LayerNew>(*this);
		}

		LayerStackControl& layers(Model::LayerStack&& l, std::vector<std::string>&& n)
		{
			m_layers = std::move(l);
			m_layer_names = std::move(n);
			m_layer_selector.clear();
			std::ranges::for_each(m_layer_names, [& layer_selector = m_layer_selector](auto const& item) {
				layer_selector.append(item.c_str());
			});
			return *this;
		}

		LayerStackControl& add(std::string&& name, Model::Layer&& layer)
		{
			m_layer_selector.append(name.c_str());
			m_layer_names.push_back(std::move(name));
			m_layers.append(std::move(layer));
			m_layer_selector.selected(m_layer_names.size() - 1);

			return *this;
		}

		template<ControlId>
		void onClicked(Ui::Button&);

		template<ControlId>
		void confirmPositive(CreateLayerDlg& dlg)
		{
			auto res = dlg.widget().create();
			add(std::move(res.first), std::move(res.second));
			m_create_layer.reset();
			m_layer_new.state(false);
		}

		template<ControlId>
		void dismiss(CreateLayerDlg&)
		{
			m_create_layer.reset();
			m_layer_new.state(false);
		}

	private:
		Model::LayerStack m_layers;
		std::vector<std::string> m_layer_names;

		Ui::Box m_root;
		Ui::Combobox m_layer_selector;
		Ui::Button m_layer_new;
		Ui::Separator m_separator_0;
		Ui::Button m_layer_copy;
		Ui::Button m_layer_link;
		Ui::Button m_layer_delete;
		Ui::Separator m_separator_1;
		Ui::Button m_layer_move_up;
		Ui::Button m_layer_move_down;
		Ui::Separator m_separator_2;
		Ui::Button m_layer_solo;
		Ui::Button m_layer_mute;
		Ui::Separator m_separator_3;
		Ui::Button m_blend_func;

		std::unique_ptr<CreateLayerDlg> m_create_layer;
	};

	template<>
	void LayerStackControl::onClicked<LayerStackControl::ControlId::LayerNew>(Ui::Button& btn)
	{
		m_create_layer = std::make_unique<CreateLayerDlg>(m_root, "Create layer");
		m_create_layer->eventHandler<LayerStackControl::ControlId::LayerNew>(*this);
	}
}

#endif
