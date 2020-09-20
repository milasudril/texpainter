//@	{
//@	"targets":[{"name":"layer_menu_handler.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_APP_LAYERMENUHANDLER_HPP
#define TEXPAINTER_APP_LAYERMENUHANDLER_HPP

#include "./menu_layer.hpp"
#include "./size_input.hpp"
#include "./layer_creator.hpp"
#include "./compositing_options_editor.hpp"

#include "utils/polymorphic_rng.hpp"
#include "utils/function_ref.hpp"
#include "model/document.hpp"
#include "ui/dialog.hpp"
#include "ui/text_entry.hpp"
#include "ui/labeled_input.hpp"

#include <random>
#include <functional>

namespace Texpainter
{
	class LayerMenuHandler
	{
		class LayerActionParams
		{
		public:
			explicit LayerActionParams(std::reference_wrapper<Model::Layer const> layer,
			                           std::reference_wrapper<std::string const> layer_name,
			                           Model::Document& doc,
			                           SimpleCallback on_completed)
			    : m_layer{layer}
			    , m_layer_name{layer_name}
			    , m_doc{doc}
			    , m_on_completed{on_completed}
			{
			}

			Model::Layer const& layer() const { return m_layer; }
			std::string const& layerName() const { return m_layer_name; }
			Model::Document& document() { return m_doc; }
			void finalize() { m_on_completed(); }

		private:
			std::reference_wrapper<Model::Layer const> m_layer;
			std::reference_wrapper<std::string const> m_layer_name;
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
		using CompositingOptionsDlg = Ui::Dialog<
		    InheritFrom<std::pair<std::reference_wrapper<Model::Document>, SimpleCallback>,
		                CompositingOptionsEditor>>;


	public:
		enum class ControlId : int
		{
			NewFromCurrentColor,
			NewFromNoise,
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

		template<LayerAction action, class MenuItemType>
		void onActivated(MenuItemType& item, Model::Document& doc, SimpleCallback on_completed)
		{
			onActivated(Tag<action>{}, item, doc, on_completed);
		}

		template<LayerActionNew action>
		void onActivated(Ui::MenuItem& item, Model::Document& doc, SimpleCallback on_completed)
		{
			onActivated(Tag<action>{}, item, doc, on_completed);
		}

		template<LayerActionClearTransformation action>
		void onActivated(Ui::MenuItem& item, Model::Document& doc, SimpleCallback on_completed)
		{
			onActivated(Tag<action>{}, item, doc, on_completed);
		}


		template<LayerAction action>
		void onActivated(Tag<action>, Ui::MenuItem&, Model::Document&, SimpleCallback on_completed)
		{
			printf("Todo: %d\n", static_cast<int>(action));
		}

		template<LayerActionNew action>
		void onActivated(Tag<action>, Ui::MenuItem&, Model::Document&, SimpleCallback)
		{
			printf("Todo: %d\n", static_cast<int>(action));
		}


		void onActivated(Tag<LayerAction::Copy>,
		                 Ui::MenuItem&,
		                 Model::Document& doc,
		                 SimpleCallback on_completed)
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

		void onActivated(Tag<LayerAction::Link>,
		                 Ui::MenuItem&,
		                 Model::Document& doc,
		                 SimpleCallback on_completed)
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

		void onActivated(Tag<LayerAction::LinkToCopy>,
		                 Ui::MenuItem&,
		                 Model::Document& doc,
		                 SimpleCallback on_completed)
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

		void onActivated(Tag<LayerAction::Rename>,
		                 Ui::MenuItem&,
		                 Model::Document& doc,
		                 SimpleCallback on_completed)
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

		void onActivated(Tag<LayerAction::Delete>,
		                 Ui::MenuItem&,
		                 Model::Document& doc,
		                 SimpleCallback on_completed)
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

		void onActivated(Tag<LayerAction::MoveToTop>,
		                 Ui::MenuItem&,
		                 Model::Document& doc,
		                 SimpleCallback on_completed)
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

		void onActivated(Tag<LayerAction::MoveUp>,
		                 Ui::MenuItem&,
		                 Model::Document& doc,
		                 SimpleCallback on_completed)
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

		void onActivated(Tag<LayerAction::MoveDown>,
		                 Ui::MenuItem&,
		                 Model::Document& doc,
		                 SimpleCallback on_completed)
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


		void onActivated(Tag<LayerAction::MoveToBottom>,
		                 Ui::MenuItem&,
		                 Model::Document& doc,
		                 SimpleCallback on_completed)
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

		void onActivated(Tag<LayerAction::CompositingOptions>,
		                 Ui::MenuItem&,
		                 Model::Document& doc,
		                 SimpleCallback on_completed)
		{
			if(auto layer = currentLayer(doc); layer != nullptr)
			{
				m_compositing_opts =
				    std::make_unique<CompositingOptionsDlg>(std::pair{std::ref(doc), on_completed},
				                                            r_dlg_owner,
				                                            "Compositing options",
				                                            layer->compositingOptions(),
				                                            layer->nodeLocations());
				m_compositing_opts->eventHandler<ControlId::CompositingOptions>(*this);
				//	m_fx_blend_editor_dlg->widget().eventHandler<ControlId::CompositingOptions>(*this);
			}
		}

		void onActivated(Tag<LayerAction::Isolate>,
		                 Ui::CheckableMenuItem& item,
		                 Model::Document&,
		                 SimpleCallback on_completed)
		{
			///	if(auto layer = currentLayer(doc); layer != nullptr) { return; }

			item.toggle();
			on_completed();
		}

		void onActivated(Tag<LayerAction::Hide>,
		                 Ui::CheckableMenuItem& item,
		                 Model::Document& doc,
		                 SimpleCallback on_completed)
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
			insertNewLayer(src.widget().inputField().content(),
			               src.widget().layer().copiedLayer(),
			               src.widget().document());
			src.widget().finalize();
			m_copy_dlg.reset();
		}

		void dismiss(Tag<ControlId::Link>, NameInputDlg&) { m_link_dlg.reset(); }


		void confirmPositive(Tag<ControlId::Link>, NameInputDlg& src)
		{
			insertNewLayer(src.widget().inputField().content(),
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
			src.widget().document().layersModify([&layer   = src.widget().layerName(),
			                                      new_name = src.widget().inputField().content(),
			                                      &doc = src.widget().document()](auto& layers) {
				layers.rename(layer, new_name);
				doc.currentLayer(new_name);
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


		void onActivated(Tag<LayerActionNew::FromCurrentColor>,
		                 Ui::MenuItem&,
		                 Model::Document& doc,
		                 SimpleCallback on_completed)
		{
			auto const size_max     = doc.canvasSize();
			auto const size_default = Size2d{size_max.width() / 2, size_max.height() / 2};
			m_new_from_color_dlg =
			    std::make_unique<LayerCreatorDlg>(std::pair{std::ref(doc), on_completed},
			                                      r_dlg_owner,
			                                      "Create new layer from current color",
			                                      size_default,
			                                      size_max);
			m_new_from_color_dlg->eventHandler<ControlId::NewFromCurrentColor>(*this);
		}

		void onActivated(Tag<LayerActionNew::FromNoise>,
		                 Ui::MenuItem&,
		                 Model::Document& doc,
		                 SimpleCallback on_completed)
		{
			auto const size_max     = doc.canvasSize();
			auto const size_default = Size2d{size_max.width() / 2, size_max.height() / 2};
			m_new_from_noise =
			    std::make_unique<LayerCreatorDlg>(std::pair{std::ref(doc), on_completed},
			                                      r_dlg_owner,
			                                      "Create new layer from noise",
			                                      size_default,
			                                      size_max);
			m_new_from_noise->eventHandler<ControlId::NewFromNoise>(*this);
		}


		void onActivated(Tag<LayerActionClearTransformation::Location>,
		                 Ui::MenuItem&,
		                 Model::Document& doc,
		                 SimpleCallback on_completed)
		{
			doc.layersModify([&current_layer = doc.currentLayer()](auto& layers) {
				if(auto layer = layers[current_layer]; layer != nullptr)
				{
					layer->location(vec2_t{0.0, 0.0});
					return true;
				}
				return false;
			});

			on_completed();
		}

		void onActivated(Tag<LayerActionClearTransformation::Rotation>,
		                 Ui::MenuItem&,
		                 Model::Document& doc,
		                 SimpleCallback on_completed)
		{
			doc.layersModify([&current_layer = doc.currentLayer()](auto& layers) {
				if(auto layer = layers[current_layer]; layer != nullptr)
				{
					layer->rotation(Angle{0.0, Angle::Turns{}});
					return true;
				}
				return false;
			});

			on_completed();
		}

		void onActivated(Tag<LayerActionClearTransformation::Scale>,
		                 Ui::MenuItem&,
		                 Model::Document& doc,
		                 SimpleCallback on_completed)
		{
			doc.layersModify([&current_layer = doc.currentLayer()](auto& layers) {
				if(auto layer = layers[current_layer]; layer != nullptr)
				{
					layer->scaleFactor(vec2_t{1.0, 1.0});
					return true;
				}
				return false;
			});

			on_completed();
		}

		void onActivated(Tag<LayerActionClearTransformation::All>,
		                 Ui::MenuItem&,
		                 Model::Document& doc,
		                 SimpleCallback on_completed)
		{
			doc.layersModify([&current_layer = doc.currentLayer()](auto& layers) {
				if(auto layer = layers[current_layer]; layer != nullptr)
				{
					layer->scaleFactor(vec2_t{1.0, 1.0})
					    .rotation(Angle{0.0, Angle::Turns{}})
					    .location(vec2_t{0.0, 0.0});
					return true;
				}
				return false;
			});

			on_completed();
		}


		void confirmPositive(Tag<ControlId::NewFromCurrentColor>, LayerCreatorDlg& src)
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
			               Model::Layer{layer_info.size, currentColor(src.widget().first.get())},
			               src.widget().first.get());
			src.widget().second();
			m_new_from_color_dlg.reset();
		}

		void dismiss(Tag<ControlId::NewFromCurrentColor>, LayerCreatorDlg&)
		{
			m_new_from_color_dlg.reset();
		}

		void confirmPositive(Tag<ControlId::NewFromNoise>, LayerCreatorDlg& src)
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

			PixelStore::Image noise{layer_info.size};
			std::ranges::generate(noise.pixels(), [rng = m_rng]() mutable {
				std::uniform_real_distribution U{0.0f, 1.0f};
				return PixelStore::Pixel{U(rng), U(rng), U(rng), U(rng)};
			});

			insertNewLayer(std::move(layer_info.name),
			               Model::Layer{std::move(noise)},
			               src.widget().first.get());
			src.widget().second();
			m_new_from_noise.reset();
		}

		void dismiss(Tag<ControlId::NewFromNoise>, LayerCreatorDlg&) { m_new_from_noise.reset(); }

		void confirmPositive(Tag<ControlId::CompositingOptions>, CompositingOptionsDlg& src)
		{
			auto& doc = src.widget().first.get();
			doc.layersModify([&current_layer = doc.currentLayer(),
			                  result = src.widget().compositingOptions()](auto& layers) mutable {
				if(auto layer = layers[current_layer]; layer != nullptr)
				{
					layer->compositingOptions(std::move(result));
					return true;
				}
				return false;
			});

			src.widget().second();
			m_compositing_opts.reset();
		}

		void dismiss(Tag<ControlId::CompositingOptions>, CompositingOptionsDlg&)
		{
			m_compositing_opts.reset();
		}

	private:
		Ui::Container& r_dlg_owner;
		PolymorphicRng m_rng;

		std::unique_ptr<NameInputDlg> m_copy_dlg;
		std::unique_ptr<NameInputDlg> m_link_dlg;
		std::unique_ptr<ConfirmationDlg> m_link_to_copy_dlg;
		std::unique_ptr<NameInputDlg> m_rename_dlg;
		std::unique_ptr<ConfirmationDlg> m_delete_dlg;

		std::unique_ptr<LayerCreatorDlg> m_new_from_color_dlg;
		std::unique_ptr<LayerCreatorDlg> m_new_from_noise;

		std::unique_ptr<CompositingOptionsDlg> m_compositing_opts;

		void insertNewLayer(std::string&& layer_name, Model::Layer&& layer, Model::Document& doc)
		{
			doc.layersModify([layer_name, &layer](auto& layers) mutable noexcept {
				// FIXME: Unique name generator...
				layers.insert(std::make_pair(std::move(layer_name), std::move(layer)));
				return true;
			});
			doc.currentLayer(std::move(layer_name));
		}
	};
}

#endif