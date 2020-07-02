//@	{"targets":[{"name":"layerstack_control.hpp", "type":"include"}]}

#ifndef TEXPAINTER_LAYERSTACKCONTROL_HPP
#define TEXPAINTER_LAYERSTACKCONTROL_HPP

#include "./layer_creator.hpp"
#include "model/layer_stack.hpp"
#include "ui/box.hpp"
#include "ui/separator.hpp"

namespace Texpainter
{
	class LayerStackControl
	{
		using CreateLayerDlg = Ui::Dialog<LayerCreator>;
		using TextInputDlg = Ui::Dialog<Ui::LabeledInput<Ui::TextEntry>>;
		using ConfirmationDlg = Ui::Dialog<Ui::Label, Ui::DialogYesNo>;

	public:
		enum class ControlId : int
		{
			LayerSelector,
			LayerNew,
			LayerCopy,
			LayerLink,
			LayerDelete,
			LayerMoveUp,
			LayerMoveDown
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
			m_layer_selector.eventHandler<ControlId::LayerSelector>(*this);
			m_layer_new.eventHandler<ControlId::LayerNew>(*this);
			m_layer_copy.eventHandler<ControlId::LayerCopy>(*this);
			m_layer_link.eventHandler<ControlId::LayerLink>(*this);
			m_layer_delete.eventHandler<ControlId::LayerDelete>(*this);
			m_layer_move_up.eventHandler<ControlId::LayerMoveUp>(*this);
			m_layer_move_down.eventHandler<ControlId::LayerMoveDown>(*this);
		}

		LayerStackControl& layers(Model::LayerStack&& l, Sequence<std::string, Model::LayerIndex>&& n)
		{
			m_layers = std::move(l);
			m_layer_names = std::move(n);
			m_current_layer = m_layers.lastIndex();
			updateLayerSelector();
			return *this;
		}

		Model::LayerStack const& layers() const
		{
			return m_layers;
		}

		LayerStackControl& add(std::string&& name, Model::Layer&& layer)
		{
			m_layer_selector.append(name.c_str());
			m_layer_names.append(std::move(name));
			m_layers.append(std::move(layer));
			auto const index = m_layer_names.size() - 1;
			m_current_layer = Model::LayerIndex{static_cast<uint32_t>(index)};
			updateLayerSelector();
			return *this;
		}

		template<class... Args>
		LayerStackControl& paintCurrentLayer(Args&&... args)
		{
			if(m_current_layer.valid()) { m_layers[m_current_layer].paint(std::forward<Args>(args)...); }
			return *this;
		}

		template<auto id, class EventHandler>
		LayerStackControl& eventHandler(EventHandler& eh)
		{
			r_eh = &eh;
			r_func = [](void* event_handler, LayerStackControl& self) {
				reinterpret_cast<EventHandler*>(event_handler)->template onChanged<id>(self);
			};

			return *this;
		}

		template<ControlId>
		void onChanged(Ui::Combobox&);

		template<ControlId>
		void onClicked(Ui::Button&);

		template<ControlId>
		void confirmPositive(CreateLayerDlg& dlg)
		{
			// FIXME: resulting layer must be smaller than canvas size
			// FIXME: handle duplicated names
			auto res = dlg.widget().create();
			add(std::move(res.first), std::move(res.second));
			m_create_layer.reset();
			m_layer_new.state(false);
			notify();
		}

		template<ControlId>
		void dismiss(CreateLayerDlg&)
		{
			m_create_layer.reset();
			m_layer_new.state(false);
		}

		template<ControlId>
		void confirmPositive(TextInputDlg&);

		template<ControlId>
		void dismiss(TextInputDlg&);

		template<ControlId>
		void confirmPositive(ConfirmationDlg&);

		template<ControlId>
		void confirmNegative(ConfirmationDlg&);


	private:
		Model::LayerIndex m_current_layer;
		Model::LayerStack m_layers;
		Sequence<std::string, Model::LayerIndex> m_layer_names;

		using EventHandlerFunc = void (*)(void*, LayerStackControl&);
		void* r_eh;
		EventHandlerFunc r_func;

		void notify()
		{
			if(r_eh != nullptr) { r_func(r_eh, *this); }
		}

		void updateLayerSelector()
		{
			m_layer_selector.clear();
			std::ranges::for_each(std::ranges::reverse_view{m_layer_names},
			                      [& layer_selector = m_layer_selector](auto const& item) {
				                      layer_selector.append(item.c_str());
			                      });
			m_layer_selector.selected(m_layers.lastIndex().value() - m_current_layer.value());
		}

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
		std::unique_ptr<TextInputDlg> m_copy_layer;
		std::unique_ptr<TextInputDlg> m_link_layer;
		std::unique_ptr<ConfirmationDlg> m_delete_layer;
	};

	template<>
	void LayerStackControl::onChanged<LayerStackControl::ControlId::LayerSelector>(Ui::Combobox& src)
	{
		auto const selected = static_cast<uint32_t>(src.selected());
		if(selected < m_layers.size()) { m_current_layer = Model::LayerIndex{m_layers.lastIndex().value() - selected}; }
	}

	template<>
	void LayerStackControl::onClicked<LayerStackControl::ControlId::LayerNew>(Ui::Button& btn)
	{
		m_create_layer = std::make_unique<CreateLayerDlg>(m_root, "Create layer");
		m_create_layer->eventHandler<LayerStackControl::ControlId::LayerNew>(*this);
	}

	template<>
	void LayerStackControl::onClicked<LayerStackControl::ControlId::LayerCopy>(Ui::Button& btn)
	{
		m_copy_layer = std::make_unique<TextInputDlg>(
		   m_root, "Copy layer", Ui::Box::Orientation::Horizontal, "New name");
		m_copy_layer->eventHandler<LayerStackControl::ControlId::LayerCopy>(*this);
	}

	template<>
	void LayerStackControl::confirmPositive<LayerStackControl::ControlId::LayerCopy>(TextInputDlg& src)
	{
		if(m_current_layer.valid())
		{
			// FIXME: handle duplicated names
			add(src.widget().inputField().content(), m_layers[m_current_layer].copiedLayer());
			m_copy_layer.reset();
			m_layer_copy.state(false);
			notify();
		}
	}

	template<>
	void LayerStackControl::dismiss<LayerStackControl::ControlId::LayerCopy>(TextInputDlg&)
	{
		m_copy_layer.reset();
		m_layer_copy.state(false);
		notify();
	}

	template<>
	void LayerStackControl::onClicked<LayerStackControl::ControlId::LayerLink>(Ui::Button& btn)
	{
		m_link_layer = std::make_unique<TextInputDlg>(
		   m_root, "Link layer", Ui::Box::Orientation::Horizontal, "New name");
		m_link_layer->eventHandler<LayerStackControl::ControlId::LayerLink>(*this);
	}

	template<>
	void LayerStackControl::confirmPositive<LayerStackControl::ControlId::LayerLink>(TextInputDlg& src)
	{
		if(m_current_layer.valid())
		{
			// FIXME: handle duplicated names
			add(src.widget().inputField().content(), m_layers[m_current_layer].linkedLayer());
			m_link_layer.reset();
			m_layer_link.state(false);
			notify();
		}
	}

	template<>
	void LayerStackControl::dismiss<LayerStackControl::ControlId::LayerLink>(TextInputDlg&)
	{
		m_link_layer.reset();
		m_layer_link.state(false);
		notify();
	}

	template<>
	void LayerStackControl::onClicked<LayerStackControl::ControlId::LayerMoveUp>(Ui::Button& btn)
	{
		if(m_current_layer.valid() && m_current_layer != m_layers.lastIndex())
		{
			m_layers.moveBackward(m_current_layer);
			m_layer_names.moveBackward(m_current_layer);
			++m_current_layer;
			updateLayerSelector();
			notify();
		}
		btn.state(false);
	}

	template<>
	void LayerStackControl::onClicked<LayerStackControl::ControlId::LayerMoveDown>(Ui::Button& btn)
	{
		if(m_current_layer.valid() && m_current_layer != m_layers.firstIndex())
		{
			m_layers.moveForward(m_current_layer);
			m_layer_names.moveForward(m_current_layer);
			--m_current_layer;
			updateLayerSelector();
			notify();
		}
		btn.state(false);
	}


	template<>
	void LayerStackControl::onClicked<LayerStackControl::ControlId::LayerDelete>(Ui::Button& btn)
	{
		if(m_current_layer.valid())
		{
			std::string msg{"Do you wish to delete `"};
			msg += m_layer_names[m_current_layer];
			msg += "`?";
			m_delete_layer = std::make_unique<ConfirmationDlg>(m_root, "Deleting layer", msg.c_str());
			m_delete_layer->eventHandler<ControlId::LayerDelete>(*this);
		}
	}

	template<>
	void LayerStackControl::confirmPositive<LayerStackControl::ControlId::LayerDelete>(
	   ConfirmationDlg&)
	{
		auto const current_layer_new = [](auto const& layers, auto layer_index) {
			if(layers.size() <= 1) { return Model::LayerIndex{}; }

			if(layer_index == layers.firstIndex()) { return layer_index; }

			return Model::LayerIndex{layer_index.value() - 1};
		}(m_layers, m_current_layer);

		m_layers.remove(m_current_layer);
		m_layer_names.remove(m_current_layer);
		m_delete_layer.reset();
		m_layer_delete.state(false);
		m_current_layer = current_layer_new;
		updateLayerSelector();
		notify();
	}

	template<>
	void LayerStackControl::confirmNegative<LayerStackControl::ControlId::LayerDelete>(
	   ConfirmationDlg&)
	{
		m_delete_layer.reset();
		m_layer_delete.state(false);
	}
}

#endif