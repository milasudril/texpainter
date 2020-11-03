//@	{
//@	"targets":[{"name":"layer_menu_handler.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_APP_LAYERMENUHANDLER_HPP
#define TEXPAINTER_APP_LAYERMENUHANDLER_HPP

#include "./menu_layer.hpp"
#include "./size_input.hpp"
#include "./layer_creator.hpp"
#include "./compositing_options_editor.hpp"
#include "./menu_action_callback.hpp"

#include "utils/polymorphic_rng.hpp"
#include "model/document.hpp"
#include "ui/dialog.hpp"
#include "ui/text_entry.hpp"
#include "ui/labeled_input.hpp"
#include "ui/error_message_dialog.hpp"
#include "pixel_store/image_io.hpp"

#include <random>
#include <functional>

namespace Texpainter
{
	template<>
	class MenuActionCallback<LayerAction::CompositingOptions>
	{
	public:
		template<class T>
		explicit MenuActionCallback(T& handler) requires(
		    !std::same_as<std::decay_t<T>, MenuActionCallback>)
		    : r_handler{&handler}
		    , on_completed{[](void* handle) {
			    auto& self = *static_cast<T*>(handle);
			    self(Tag<LayerAction::CompositingOptions>{});
		    }}
		    , on_changed{[](void* handle, CompositingOptionsEditor const& editor) {
			    auto& self = *static_cast<T*>(handle);
			    self.template onUpdated<LayerAction::CompositingOptions>(editor);
		    }}
		{
		}

		void onCompleted() const { on_completed(r_handler); }

		template<auto>
		void onChanged(CompositingOptionsEditor const& editor) const
		{
			on_changed(r_handler, editor);
		}


	private:
		void* r_handler;
		void (*on_completed)(void*);
		void (*on_changed)(void*, CompositingOptionsEditor const& editor);
	};

	class LayerMenuHandler
	{
		class LayerActionParams
		{
		public:
			explicit LayerActionParams(std::reference_wrapper<Model::Layer const> layer,
			                           std::reference_wrapper<Model::ItemName const> layer_name,
			                           Model::Document& doc,
			                           SimpleCallback on_completed)
			    : m_layer{layer}
			    , m_layer_name{layer_name}
			    , m_doc{doc}
			    , m_on_completed{on_completed}
			{
			}

			Model::Layer const& layer() const { return m_layer; }
			Model::ItemName const& layerName() const { return m_layer_name; }
			Model::Document& document() { return m_doc; }
			void finalize() { m_on_completed(); }

		private:
			std::reference_wrapper<Model::Layer const> m_layer;
			std::reference_wrapper<Model::ItemName const> m_layer_name;
			std::reference_wrapper<Model::Document> m_doc;
			SimpleCallback m_on_completed;
		};


		using NameInputDlg =
		    Texpainter::Ui::Dialog<InheritFrom<LayerActionParams, Ui::LabeledInput<Ui::TextEntry>>>;
		using ConfirmationDlg =
		    Ui::Dialog<InheritFrom<LayerActionParams, Ui::Label>, Ui::DialogYesNo>;
		using LayerCreatorDlg = Ui::Dialog<
		    InheritFrom<std::pair<std::reference_wrapper<Model::Document>, SimpleCallback>,
		                LayerCreator>>;
		using CompositingOptionsDlg =
		    Ui::Dialog<InheritFrom<std::pair<std::reference_wrapper<Model::Document>,
		                                     MenuActionCallback<LayerAction::CompositingOptions>>,
		                           CompositingOptionsEditor>>;


	public:
		enum class ControlId : int
		{
			NewEmpty,
			Copy,
			Link,
			LinkToCopy,
			Rename,
			Delete,
			CompositingOptions
		};

		explicit LayerMenuHandler(Ui::Container& dialog_owner, PolymorphicRng rng)
		    : r_dlg_owner{dialog_owner}
		    , m_rng{rng}
		{
		}

		void onActivated(Ui::MenuItem&,
		                 Model::Document& doc,
		                 MenuActionCallback<LayerAction::Copy> on_completed)
		{
			if(auto layer = currentLayer(doc); layer != nullptr)
			{
				m_copy_dlg = std::make_unique<NameInputDlg>(
				    LayerActionParams{*layer, doc.currentLayer(), doc, on_completed},
				    r_dlg_owner,
				    "Copy current layer",
				    Ui::Box::Orientation::Horizontal,
				    "Name: ");
				m_copy_dlg->template eventHandler<ControlId::Copy>(*this);
			}
		}

		void onActivated(Ui::MenuItem&,
		                 Model::Document& doc,
		                 MenuActionCallback<LayerAction::Link> on_completed)
		{
			if(auto layer = currentLayer(doc); layer != nullptr)
			{
				m_link_dlg = std::make_unique<NameInputDlg>(
				    LayerActionParams{*layer, doc.currentLayer(), doc, on_completed},
				    r_dlg_owner,
				    "Create link to current layer",
				    Ui::Box::Orientation::Horizontal,
				    "Name: ");
				m_link_dlg->template eventHandler<ControlId::Link>(*this);
			}
		}

		void onActivated(Ui::MenuItem&,
		                 Model::Document& doc,
		                 MenuActionCallback<LayerAction::LinkToCopy> on_completed)
		{
			if(auto layer = currentLayer(doc); layer != nullptr)
			{
				m_link_to_copy_dlg = std::make_unique<ConfirmationDlg>(
				    LayerActionParams{*layer, doc.currentLayer(), doc, on_completed},
				    r_dlg_owner,
				    "Convert linked layer",
				    "Are you shure you want to convert current layer to a static copy?");
				m_link_to_copy_dlg->template eventHandler<ControlId::LinkToCopy>(*this);
			}
		}

		void onActivated(Ui::MenuItem&,
		                 Model::Document& doc,
		                 MenuActionCallback<LayerAction::Rename> on_completed)
		{
			if(auto layer = currentLayer(doc); layer != nullptr)
			{
				m_rename_dlg = std::make_unique<NameInputDlg>(
				    LayerActionParams{*layer, doc.currentLayer(), doc, on_completed},
				    r_dlg_owner,
				    "Rename current layer",
				    Ui::Box::Orientation::Horizontal,
				    "Name: ");
				m_rename_dlg->template eventHandler<ControlId::Rename>(*this);
			}
		}

		void onActivated(Ui::MenuItem&,
		                 Model::Document& doc,
		                 MenuActionCallback<LayerAction::Delete> on_completed)
		{
			if(auto layer = currentLayer(doc); layer != nullptr)
			{
				m_delete_dlg = std::make_unique<ConfirmationDlg>(
				    LayerActionParams{*layer, doc.currentLayer(), doc, on_completed},
				    r_dlg_owner,
				    "Delete layer",
				    "Are you shure you want to delete current layer?");
				m_delete_dlg->template eventHandler<ControlId::Delete>(*this);
			}
		}

		void onActivated(Ui::MenuItem&,
		                 Model::Document& doc,
		                 MenuActionCallback<LayerAction::MoveToTop> on_completed)
		{
			doc.layersModify([&current_layer = doc.currentLayer()](auto& layers) {
				if(auto i = layers.index(current_layer); i.valid())
				{
					layers.moveBack(i);
					return true;
				}
				return false;
			});
			on_completed();
		}

		void onActivated(Ui::MenuItem&,
		                 Model::Document& doc,
		                 MenuActionCallback<LayerAction::MoveUp> on_completed)
		{
			doc.layersModify([&current_layer = doc.currentLayer()](auto& layers) {
				if(auto i = layers.index(current_layer); i.valid())
				{
					layers.moveBackward(i);
					return true;
				}
				return false;
			});
			on_completed();
		}

		void onActivated(Ui::MenuItem&,
		                 Model::Document& doc,
		                 MenuActionCallback<LayerAction::MoveDown> on_completed)
		{
			doc.layersModify([&current_layer = doc.currentLayer()](auto& layers) {
				if(auto i = layers.index(current_layer); i.valid())
				{
					layers.moveForward(i);
					return true;
				}
				return false;
			});
			on_completed();
		}


		void onActivated(Ui::MenuItem&,
		                 Model::Document& doc,
		                 MenuActionCallback<LayerAction::MoveToBottom> on_completed)
		{
			doc.layersModify([&current_layer = doc.currentLayer()](auto& layers) {
				if(auto i = layers.index(current_layer); i.valid())
				{
					layers.moveFront(i);
					return true;
				}
				return false;
			});
			on_completed();
		}

		void onActivated(Ui::MenuItem&,
		                 Model::Document& doc,
		                 MenuActionCallback<LayerAction::CompositingOptions> on_completed)
		{
			if(auto layer = currentLayer(doc); layer != nullptr)
			{
				m_compositing_opts =
				    std::make_unique<CompositingOptionsDlg>(std::pair{std::ref(doc), on_completed},
				                                            r_dlg_owner,
				                                            "Compositing options",
				                                            layer->compositingOptions(),
				                                            layer->nodeLocations());
				m_compositing_opts->eventHandler<ControlId::CompositingOptions>(*this)
				    .widget()
				    .template eventHandler<0>(m_compositing_opts->widget().second);
			}
		}

		void onActivated(Ui::CheckableMenuItem& item,
		                 Model::Document& doc,
		                 MenuActionCallback<LayerAction::Isolate> on_completed)
		{
			doc.layersModify([&current_layer = doc.currentLayer(), &item](auto& layers) {
				if(auto layer = layers[current_layer]; layer != nullptr)
				{
					layer->isolated(item.status());
					return true;
				}
				item.toggle();
				return false;
			});
			on_completed();
		}

		void onActivated(Ui::CheckableMenuItem& item,
		                 Model::Document& doc,
		                 MenuActionCallback<LayerAction::Hide> on_completed)
		{
			doc.layersModify([&current_layer = doc.currentLayer(), &item](auto& layers) {
				if(auto layer = layers[current_layer]; layer != nullptr)
				{
					layer->visible(!item.status());
					return true;
				}
				item.toggle();
				return false;
			});
			on_completed();
		}

		template<ControlId id, class Src>
		void dismiss(Src& src)
		{
			dismiss(Tag<id>{}, src);
		}

		template<ControlId id, class Src>
		void confirmPositive(Src& src)
		{
			confirmPositive(Tag<id>{}, src);
		}

		template<ControlId id, class Src>
		void confirmNegative(Src& src)
		{
			confirmNegative(Tag<id>{}, src);
		}


		void confirmPositive(Tag<ControlId::Copy>, NameInputDlg& src)
		{
			insertNewLayer(Model::ItemName{src.widget().inputField().content()},
			               src.widget().layer().copiedLayer(),
			               src.widget().document());
			src.widget().finalize();
			m_copy_dlg.reset();
		}

		void dismiss(Tag<ControlId::Link>, NameInputDlg&) { m_link_dlg.reset(); }


		void confirmPositive(Tag<ControlId::Link>, NameInputDlg& src)
		{
			insertNewLayer(Model::ItemName{src.widget().inputField().content()},
			               src.widget().layer().linkedLayer(),
			               src.widget().document());
			src.widget().finalize();
			m_link_dlg.reset();
		}

		void dismiss(Tag<ControlId::Copy>, NameInputDlg&) { m_copy_dlg.reset(); }


		void confirmPositive(Tag<ControlId::LinkToCopy>, ConfirmationDlg& src)
		{
			src.widget().document().layersModify([&layer = src.widget().layerName()](auto& layers) {
				layers[layer]->convertToCopy();
				return true;
			});

			src.widget().finalize();
			m_link_to_copy_dlg.reset();
		}

		void confirmNegative(Tag<ControlId::LinkToCopy>, ConfirmationDlg&)
		{
			m_link_to_copy_dlg.reset();
		}


		void confirmPositive(Tag<ControlId::Rename>, NameInputDlg& src)
		{
			src.widget().document().layersModify(
			    [&layer   = src.widget().layerName(),
			     new_name = Model::ItemName{src.widget().inputField().content()},
			     &doc     = src.widget().document()](auto& layers) {
				    renameOrThrow(layers, layer, Model::ItemName{new_name});
				    doc.currentLayer(Model::ItemName{new_name});
				    return true;
			    });

			src.widget().finalize();
			m_rename_dlg.reset();
		}

		void dismiss(Tag<ControlId::Rename>, NameInputDlg&) { m_rename_dlg.reset(); }


		void confirmPositive(Tag<ControlId::Delete>, ConfirmationDlg& src)
		{
			src.widget().document().layersModify(
			    [&layer = src.widget().layerName(), &doc = src.widget().document()](auto& layers) {
				    auto i = layers.index(layer);
				    if(i != Model::LayerIndex{0}) { --i; }
				    else if(i.value() + 1 != layers.size().value())
				    {
					    ++i;
				    }
				    auto current_layer_copy = *layers.key(i);
				    layers.erase(layer);
				    if(layers.size() != Model::LayerIndex{0})
				    { doc.currentLayer(std::move(current_layer_copy)); }
				    return true;
			    });

			src.widget().finalize();
			m_delete_dlg.reset();
		}

		void confirmNegative(Tag<ControlId::Delete>, ConfirmationDlg&) { m_delete_dlg.reset(); }

		template<auto action>
		void onActivated(Ui::MenuItem&, Model::Document&, MenuActionCallback<action>)
		{
		}

		void onActivated(Ui::MenuItem&,
		                 Model::Document& doc,
		                 MenuActionCallback<LayerActionNew::Empty> on_completed)
		{
			auto const size_max     = doc.canvasSize();
			auto const size_default = Size2d{size_max.width() / 2, size_max.height() / 2};
			m_new_empty_dlg =
			    std::make_unique<LayerCreatorDlg>(std::pair{std::ref(doc), on_completed},
			                                      r_dlg_owner,
			                                      "Create new layer from current color",
			                                      size_default,
			                                      size_max);
			m_new_empty_dlg->eventHandler<ControlId::NewEmpty>(*this);
		}

		void onActivated(Ui::MenuItem&,
		                 Model::Document& doc,
		                 MenuActionCallback<LayerActionNew::FromFile> on_completed)
		{
			Model::Layer layer{PixelStore::load("test_pattern/test_pattern.exr")};
			insertNewLayer(Model::ItemName{"Foo"}, std::move(layer), doc);
			on_completed();
		}

		void onActivated(Ui::MenuItem&,
		                 Model::Document& doc,
		                 MenuActionCallback<LayerActionClearTransformation::Location> on_completed)
		{
			(void)modifyCurrentLayer(doc, [](auto& layer) {
				layer.location(vec2_t{0.0, 0.0});
				return true;
			});

			on_completed();
		}

		void onActivated(Ui::MenuItem&,
		                 Model::Document& doc,
		                 MenuActionCallback<LayerActionClearTransformation::Rotation> on_completed)
		{
			(void)modifyCurrentLayer(doc, [](auto& layer) {
				layer.rotation(Angle{0.0, Angle::Turns{}});
				return true;
			});

			on_completed();
		}

		void onActivated(Ui::MenuItem&,
		                 Model::Document& doc,
		                 MenuActionCallback<LayerActionClearTransformation::Scale> on_completed)
		{
			(void)modifyCurrentLayer(doc, [](auto& layer) {
				layer.scaleFactor(vec2_t{1.0, 1.0});
				return true;
			});

			on_completed();
		}

		void onActivated(Ui::MenuItem&,
		                 Model::Document& doc,
		                 MenuActionCallback<LayerActionClearTransformation::All> on_completed)
		{
			(void)modifyCurrentLayer(doc, [](auto& layer) {
				layer.scaleFactor(vec2_t{1.0, 1.0})
				    .rotation(Angle{0.0, Angle::Turns{}})
				    .location(vec2_t{0.0, 0.0});
				return true;
			});

			on_completed();
		}


		void confirmPositive(Tag<ControlId::NewEmpty>, LayerCreatorDlg& src)
		{
			auto layer_info = src.widget().value();
			if(!isSupported<PixelStore::Pixel>(layer_info.size))
			{
				// FIXME:
				//   "A layer of this size cannot be created. The number of bytes required to create a layer
				//   of "
				//  "this size exeeds the largest supported integer value."};
				return;
			}
			insertNewLayer(std::move(layer_info.name),
			               Model::Layer{layer_info.size, PixelStore::Pixel{0.0f, 0.0f, 0.0f, 0.0f}},
			               src.widget().first.get());
			src.widget().second();
			m_new_empty_dlg.reset();
		}

		void dismiss(Tag<ControlId::NewEmpty>, LayerCreatorDlg&) { m_new_empty_dlg.reset(); }

		void confirmPositive(Tag<ControlId::CompositingOptions>, CompositingOptionsDlg& src)
		{
			auto& doc = src.widget().first.get();
			modifyCurrentLayer(
			    doc,
			    [result         = src.widget().compositingOptionsWithGraph(),
			     node_locations = src.widget().nodeLocations()](auto& layer) mutable {
				    layer.nodeLocations(std::move(node_locations))
				        .compositingOptions(std::move(result));
				    return true;
			    });

			src.widget().second.onCompleted();
			m_compositing_opts.reset();
		}

		void dismiss(Tag<ControlId::CompositingOptions>, CompositingOptionsDlg&)
		{
			m_compositing_opts.reset();
		}

		template<auto id, class DialogType>
		void handleException(char const* msg, DialogType& src) requires requires
		{
			{
				src.owner()
			}
			->std::same_as<Ui::Window&>;
		}
		{
			m_err_display.show(src.owner(), "Texpainter", msg);
		}

	private:
		Ui::Container& r_dlg_owner;
		PolymorphicRng m_rng;

		std::unique_ptr<NameInputDlg> m_copy_dlg;
		std::unique_ptr<NameInputDlg> m_link_dlg;
		std::unique_ptr<ConfirmationDlg> m_link_to_copy_dlg;
		std::unique_ptr<NameInputDlg> m_rename_dlg;
		std::unique_ptr<ConfirmationDlg> m_delete_dlg;

		std::unique_ptr<LayerCreatorDlg> m_new_empty_dlg;

		std::unique_ptr<CompositingOptionsDlg> m_compositing_opts;

		Ui::ErrorMessageDialog m_err_display;

		void insertNewLayer(Model::ItemName&& layer_name,
		                    Model::Layer&& layer,
		                    Model::Document& doc)
		{
			doc.layersModify([layer_name, &layer](auto& layers) {
				insertOrThrow(layers, layer_name, std::move(layer));
				return true;
			});
			doc.currentLayer(std::move(layer_name));
		}
	};
}

#endif