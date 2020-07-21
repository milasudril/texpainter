//@	{
//@	 "targets":[{"name":"layerstack_control.hpp", "type":"include"}]
//@	,"dependencies_extra":[{"ref":"layerstack_control.o","rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_LAYERSTACKCONTROL_HPP
#define TEXPAINTER_LAYERSTACKCONTROL_HPP

#include "./layer_creator.hpp"

#include "model/layer_stack.hpp"
#include "ui/dialog.hpp"

namespace Texpainter
{
	class LayerStackControl
	{
		using CreateLayerDlg =
		    Ui::Dialog<LayerCreator, Ui::DialogOkCancel, Ui::InitialFocus::NoChange>;
		using TextInputDlg    = Ui::Dialog<Ui::LabeledInput<Ui::TextEntry>>;
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

		explicit LayerStackControl(Ui::Container& owner, Size2d canvas_size)
		    : m_canvas_size{canvas_size}
		{
		}

		LayerStackControl& layers(Model::LayerStack&& l,
		                          Sequence<std::string, Model::LayerIndex>&& n)
		{
			m_layers        = std::move(l);
			m_layer_names   = std::move(n);
			m_current_layer = m_layers.lastIndex();
			return *this;
		}

		Model::LayerStack const& layers() const { return m_layers; }

		LayerStackControl& add(std::string&& name, Model::Layer&& layer)
		{
			m_layer_names.append(std::move(name));
			m_layers.append(std::move(layer));
			auto const index = m_layer_names.size() - 1;
			m_current_layer  = Model::LayerIndex{static_cast<uint32_t>(index)};
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
			{ m_layers[m_current_layer].paint(std::forward<Args>(args)...); }
			return *this;
		}

		LayerStackControl& moveCurrentLayer(vec2_t pos)
		{
			if(m_current_layer.valid()) { m_layers[m_current_layer].location(pos); }
			return *this;
		}

		template<class ScaleConstraint>
		LayerStackControl& scaleCurrentLayer(vec2_t loc,
		                                     vec2_t origin,
		                                     ScaleConstraint&& constraint)
		{
			if(m_current_layer.valid())
			{
				auto const layer_loc = m_layers[m_current_layer].location();
				auto factor          = constraint((loc - layer_loc) / (origin - layer_loc));
				m_layers[m_current_layer].scaleFactor(factor);
			}
			return *this;
		}

		template<class RotConstraint>
		LayerStackControl& rotateCurrentLayer(vec2_t loc, RotConstraint&& constraint)
		{
			if(m_current_layer.valid())
			{
				auto const layer_loc = m_layers[m_current_layer].location();
				auto const ϴ         = constraint(Angle{loc - layer_loc});
				m_layers[m_current_layer].rotation(ϴ);
			}
			return *this;
		}

		LayerStackControl& outlineCurrentLayer(Span2d<Model::Pixel> canvas);

		template<auto id, class EventHandler>
		LayerStackControl& eventHandler(EventHandler& eh)
		{
			r_eh   = &eh;
			r_func = [](void* event_handler, LayerStackControl& self) {
				reinterpret_cast<EventHandler*>(event_handler)->template onChanged<id>(self);
			};

			return *this;
		}

		template<ControlId>
		void confirmPositive(CreateLayerDlg& dlg)
		{
			// FIXME: handle duplicated names
			try
			{
				auto res = dlg.widget().create();
				add(std::move(res.first), std::move(res.second));
				m_create_layer.reset();
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

		std::unique_ptr<CreateLayerDlg> m_create_layer;
		std::unique_ptr<TextInputDlg> m_copy_layer;
		std::unique_ptr<TextInputDlg> m_link_layer;
		std::unique_ptr<ConfirmationDlg> m_delete_layer;
	};

	template<>
	inline void LayerStackControl::confirmPositive<LayerStackControl::ControlId::LayerCopy>(
	    TextInputDlg& src)
	{
		if(m_current_layer.valid())
		{
			// FIXME: handle duplicated names
			add(src.widget().inputField().content(), m_layers[m_current_layer].copiedLayer());
			m_copy_layer.reset();
			notify();
		}
	}

	template<>
	inline void LayerStackControl::dismiss<LayerStackControl::ControlId::LayerCopy>(TextInputDlg&)
	{
		m_copy_layer.reset();
		notify();
	}

	template<>
	inline void LayerStackControl::confirmPositive<LayerStackControl::ControlId::LayerLink>(
	    TextInputDlg& src)
	{
		if(m_current_layer.valid())
		{
			// FIXME: handle duplicated names
			add(src.widget().inputField().content(), m_layers[m_current_layer].linkedLayer());
			m_link_layer.reset();
			notify();
		}
	}

	template<>
	inline void LayerStackControl::dismiss<LayerStackControl::ControlId::LayerLink>(TextInputDlg&)
	{
		m_link_layer.reset();
		notify();
	}

	template<>
	inline void LayerStackControl::confirmPositive<LayerStackControl::ControlId::LayerDelete>(
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
		m_current_layer = current_layer_new;
		notify();
	}

	template<>
	inline void LayerStackControl::confirmNegative<LayerStackControl::ControlId::LayerDelete>(
	    ConfirmationDlg&)
	{
		m_delete_layer.reset();
	}
}

#endif