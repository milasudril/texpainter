//@	{
//@	 "targets":[{"name":"layerstack_control.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"layerstack_control.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_LAYERSTACKCONTROL_HPP
#define TEXPAINTER_LAYERSTACKCONTROL_HPP

#include "./layer_creator.hpp"
#include "./snap.hpp"

#include "model/layer_stack.hpp"
#include "ui/box.hpp"
#include "ui/separator.hpp"
#include "ui/dialog.hpp"
#include "ui/combobox.hpp"

namespace Texpainter
{
	class LayerStackControl
	{
		using CreateLayerDlg = Ui::Dialog<LayerCreator, Ui::DialogOkCancel, Ui::InitialFocus::NoChange>;
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
			LayerMoveDown,
			LayerHide
		};

		explicit LayerStackControl(Ui::Container& owner, Size2d canvas_size):
		   m_canvas_size{canvas_size},
		   m_root{owner, Ui::Box::Orientation::Vertical},
		   m_row_1{m_root, Ui::Box::Orientation::Horizontal},
		   m_layer_selector{m_row_1},
		   m_layer_new{m_row_1, "＋"},
		   m_separator_0{m_row_1},
		   m_layer_copy{m_row_1, "Copy"},
		   m_layer_link{m_row_1, "Link"},
		   m_layer_delete{m_row_1, "−"},
		   m_separator_1{m_row_1},
		   m_layer_move_up{m_row_1, "↑"},
		   m_layer_move_down{m_row_1, "↓"},
		   m_separator_2{m_row_1},
		   m_layer_solo{m_row_1, "Solo"},
		   m_layer_hide{m_row_1, "Hide"},
		   m_separator_3{m_row_1},
		   m_blend_func{m_row_1, "f(x)"},
		   m_row_2{m_root, Ui::Box::Orientation::Horizontal},
		   m_status{m_row_2.insertMode(Ui::Box::InsertMode{0, Ui::Box::Fill | Ui::Box::Expand}), ""}
		{
			m_layer_selector.eventHandler<ControlId::LayerSelector>(*this);
			m_layer_new.eventHandler<ControlId::LayerNew>(*this);
			m_layer_copy.eventHandler<ControlId::LayerCopy>(*this);
			m_layer_link.eventHandler<ControlId::LayerLink>(*this);
			m_layer_delete.eventHandler<ControlId::LayerDelete>(*this);
			m_layer_move_up.eventHandler<ControlId::LayerMoveUp>(*this);
			m_layer_move_down.eventHandler<ControlId::LayerMoveDown>(*this);
			m_layer_hide.eventHandler<ControlId::LayerHide>(*this);
			m_status.oneline(true).alignment(0.0f);
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
			showLayerInfo(m_current_layer);
			return *this;
		}

		LayerStackControl& canvasSize(Size2d val)
		{
			m_canvas_size = val;
			return *this;
		}

		template<class... Args>
		LayerStackControl& paintCurrentLayer(Args&&... args)
		{
			if(m_current_layer.valid())
			{
				m_layers[m_current_layer].paint(std::forward<Args>(args)...);
				showLayerInfo(m_current_layer);
			}
			return *this;
		}

		LayerStackControl& moveCurrentLayer(vec2_t pos)
		{
			if(m_current_layer.valid())
			{
				m_layers[m_current_layer].location(pos);
				showLayerInfo(m_current_layer);
			}
			return *this;
		}

		template<class ScaleConstraint>
		LayerStackControl& scaleCurrentLayer(vec2_t loc, vec2_t origin, ScaleConstraint&& constraint)
		{
			if(m_current_layer.valid())
			{
				auto const layer_loc = m_layers[m_current_layer].location();
				auto factor = constraint((loc - layer_loc) / (origin - layer_loc));
				m_layers[m_current_layer].scaleFactor(factor);
				showLayerInfo(m_current_layer);
			}
			return *this;
		}

		template<class RotConstraint>
		LayerStackControl& rotateCurrentLayer(vec2_t loc, RotConstraint&& constraint)
		{
			if(m_current_layer.valid())
			{
				auto const layer_loc = m_layers[m_current_layer].location();
				auto const ϴ = constraint(Angle{loc - layer_loc});
				m_layers[m_current_layer].rotation(ϴ);
				showLayerInfo(m_current_layer);
			}
			return *this;
		}

		LayerStackControl& outlineCurrentLayer(Span2d<Model::Pixel> canvas);

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
			// FIXME: handle duplicated names
			try
			{
				auto res = dlg.widget().create();
				add(std::move(res.first), std::move(res.second));
				m_create_layer.reset();
				m_layer_new.state(false);
				notify();
			}
			catch(std::exception const& err)
			{
				printf("%s\n", err.what());
			}
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
		Size2d m_canvas_size;

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
		Ui::Box m_row_1;
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
		Ui::Button m_layer_hide;
		Ui::Separator m_separator_3;
		Ui::Button m_blend_func;
		Ui::Box m_row_2;
		Ui::Label m_status;

		std::unique_ptr<CreateLayerDlg> m_create_layer;
		std::unique_ptr<TextInputDlg> m_copy_layer;
		std::unique_ptr<TextInputDlg> m_link_layer;
		std::unique_ptr<ConfirmationDlg> m_delete_layer;

		void showLayerInfo(Model::LayerIndex index)
		{
			auto const& layer = m_layers[index];
			std::string msg{"Layer "};
			msg += std::to_string(index.value());
			msg += ". Size: ";
			msg += std::to_string(layer.size().width());
			msg += "x";
			msg += std::to_string(layer.size().height());
			msg += " Position: (";
			msg += std::to_string(layer.location()[0]);
			msg += ", ";
			msg += std::to_string(layer.location()[1]);
			msg += ") Scale: (";
			msg += std::to_string(layer.scaleFactor()[0]);
			msg += ", ";
			msg += std::to_string(layer.scaleFactor()[1]);
			msg += ") Rotation: ";
			msg += std::to_string(layer.rotation().turns());
			m_status.content(msg.c_str());
		}
	};

	template<>
	inline void
	LayerStackControl::onChanged<LayerStackControl::ControlId::LayerSelector>(Ui::Combobox& src)
	{
		auto const selected = static_cast<uint32_t>(src.selected());
		if(selected < m_layers.size())
		{
			m_current_layer = Model::LayerIndex{m_layers.lastIndex().value() - selected};
			showLayerInfo(m_current_layer);
			notify();
		}
	}

	template<>
	inline void LayerStackControl::onClicked<LayerStackControl::ControlId::LayerNew>(Ui::Button& btn)
	{
		m_create_layer = std::make_unique<CreateLayerDlg>(
		   m_row_1,
		   "Create layer",
		   m_current_layer.valid() ? m_layers[m_current_layer].size() : m_canvas_size,
		   m_canvas_size);
		m_create_layer->eventHandler<LayerStackControl::ControlId::LayerNew>(*this);
	}

	template<>
	inline void LayerStackControl::onClicked<LayerStackControl::ControlId::LayerCopy>(Ui::Button& btn)
	{
		m_copy_layer = std::make_unique<TextInputDlg>(
		   m_row_1, "Copy layer", Ui::Box::Orientation::Horizontal, "New name");
		m_copy_layer->eventHandler<LayerStackControl::ControlId::LayerCopy>(*this);
	}

	template<>
	inline void
	LayerStackControl::confirmPositive<LayerStackControl::ControlId::LayerCopy>(TextInputDlg& src)
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
	inline void LayerStackControl::dismiss<LayerStackControl::ControlId::LayerCopy>(TextInputDlg&)
	{
		m_copy_layer.reset();
		m_layer_copy.state(false);
		notify();
	}

	template<>
	inline void LayerStackControl::onClicked<LayerStackControl::ControlId::LayerLink>(Ui::Button& btn)
	{
		m_link_layer = std::make_unique<TextInputDlg>(
		   m_row_1, "Link layer", Ui::Box::Orientation::Horizontal, "New name");
		m_link_layer->eventHandler<LayerStackControl::ControlId::LayerLink>(*this);
	}

	template<>
	inline void
	LayerStackControl::confirmPositive<LayerStackControl::ControlId::LayerLink>(TextInputDlg& src)
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
	inline void LayerStackControl::dismiss<LayerStackControl::ControlId::LayerLink>(TextInputDlg&)
	{
		m_link_layer.reset();
		m_layer_link.state(false);
		notify();
	}

	template<>
	inline void
	LayerStackControl::onClicked<LayerStackControl::ControlId::LayerMoveUp>(Ui::Button& btn)
	{
		if(m_current_layer.valid() && m_current_layer != m_layers.lastIndex())
		{
			m_layers.moveBackward(m_current_layer);
			m_layer_names.moveBackward(m_current_layer);
			++m_current_layer;
			updateLayerSelector();
			showLayerInfo(m_current_layer);
			notify();
		}
		btn.state(false);
	}

	template<>
	inline void
	LayerStackControl::onClicked<LayerStackControl::ControlId::LayerMoveDown>(Ui::Button& btn)
	{
		if(m_current_layer.valid() && m_current_layer != m_layers.firstIndex())
		{
			m_layers.moveForward(m_current_layer);
			m_layer_names.moveForward(m_current_layer);
			--m_current_layer;
			updateLayerSelector();
			showLayerInfo(m_current_layer);
			notify();
		}
		btn.state(false);
	}


	template<>
	inline void
	LayerStackControl::onClicked<LayerStackControl::ControlId::LayerDelete>(Ui::Button& btn)
	{
		if(m_current_layer.valid())
		{
			std::string msg{"Do you wish to delete `"};
			msg += m_layer_names[m_current_layer];
			msg += "`?";
			m_delete_layer = std::make_unique<ConfirmationDlg>(m_row_1, "Deleting layer", msg.c_str());
			m_delete_layer->eventHandler<ControlId::LayerDelete>(*this);
		}
	}

	template<>
	inline void
	LayerStackControl::confirmPositive<LayerStackControl::ControlId::LayerDelete>(ConfirmationDlg&)
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
		showLayerInfo(m_current_layer);
		notify();
	}

	template<>
	inline void
	LayerStackControl::confirmNegative<LayerStackControl::ControlId::LayerDelete>(ConfirmationDlg&)
	{
		m_delete_layer.reset();
		m_layer_delete.state(false);
	}

	template<>
	inline void LayerStackControl::onClicked<LayerStackControl::ControlId::LayerHide>(Ui::Button& btn)
	{
		if(m_current_layer.valid())
		{
			m_layers[m_current_layer].visible(!btn.state());
			notify();
		}
		else
		{
			btn.state(false);
		}
	}
}

#endif