//@	{
//@	"targets":[{"name":"layer_menu_handler.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_LAYERMENUHANDLER_HPP
#define TEXPAINTER_LAYERMENUHANDLER_HPP

#include "./menu_layer.hpp"
#include "./size_input.hpp"
#include "./layer_creator.hpp"

#include "utils/polymorphic_rng.hpp"
#include "model/document.hpp"
#include "ui/dialog.hpp"
#include "ui/text_entry.hpp"
#include "ui/labeled_input.hpp"

#include <random>

namespace Texpainter
{
	template<class DocOwner>
	class LayerMenuHandler
	{
		template<class Widget>
		struct WidgetWithLayerRef: public Widget
		{
			template<class... Args>
			WidgetWithLayerRef(Args&&... args)
			    : Widget{std::forward<Args>(args)...}
			    , layer{nullptr}
			    , layer_name{nullptr}
			{
			}

			Model::Layer const* layer;
			std::string const* layer_name;
		};


		using NameInputDlg =
		    Texpainter::Ui::Dialog<WidgetWithLayerRef<Ui::LabeledInput<Ui::TextEntry>>>;
		using ConfirmationDlg = Ui::Dialog<WidgetWithLayerRef<Ui::Label>, Ui::DialogYesNo>;
		using LayerCreatorDlg = Ui::Dialog<LayerCreator>;

	public:
		enum class ControlId : int
		{
			NewFromCurrentColor,
			NewFromNoise,
			Copy,
			Link,
			LinkToCopy,
			Rename
		};

		explicit LayerMenuHandler(Ui::Container& dialog_owner,
		                          DocOwner& doc_owner,
		                          PolymorphicRng rng)
		    : r_dlg_owner{dialog_owner}
		    , r_doc_owner{doc_owner}
		    , m_rng{rng}
		{
		}

		template<LayerActionNew action>
		void onActivated(Ui::MenuItem& item)
		{
			onActivated(Tag<action>{}, item);
		}


		template<LayerAction action>
		void onActivated(Ui::MenuItem& item)
		{
			onActivated(Tag<action>{}, item);
		}

		template<LayerAction action>
		void onActivated(Tag<action>, Ui::MenuItem&)
		{
			printf("Todo: %d\n", static_cast<int>(action));
		}

		template<LayerActionNew action>
		void onActivated(Tag<action>, Ui::MenuItem&)
		{
			printf("Todo: %d\n", static_cast<int>(action));
		}


		void onActivated(Tag<LayerAction::Copy>, Ui::MenuItem&)
		{
			if(auto layer = currentLayer(r_doc_owner.document()); layer != nullptr)
			{
				m_copy_dlg = std::make_unique<NameInputDlg>(
				    r_dlg_owner, "Copy current layer", Ui::Box::Orientation::Horizontal, "Name: ");
				m_copy_dlg->widget().layer      = layer;
				m_copy_dlg->widget().layer_name = &r_doc_owner.document().currentLayer();
				m_copy_dlg->template eventHandler<ControlId::Copy>(*this);
			}
		}

		void onActivated(Tag<LayerAction::Link>, Ui::MenuItem&)
		{
			if(auto layer = currentLayer(r_doc_owner.document()); layer != nullptr)
			{
				m_link_dlg                      = std::make_unique<NameInputDlg>(r_dlg_owner,
                                                            "Create link to current layer",
                                                            Ui::Box::Orientation::Horizontal,
                                                            "Name: ");
				m_link_dlg->widget().layer      = layer;
				m_link_dlg->widget().layer_name = &r_doc_owner.document().currentLayer();
				m_link_dlg->template eventHandler<ControlId::Link>(*this);
			}
		}

		void onActivated(Tag<LayerAction::LinkToCopy>, Ui::MenuItem&)
		{
			if(auto layer = currentLayer(r_doc_owner.document()); layer != nullptr)
			{
				m_link_to_copy_dlg = std::make_unique<ConfirmationDlg>(
				    r_dlg_owner,
				    "Convert linked layer",
				    "Are you shure you want to convert current layer to a static copy?");
				m_link_to_copy_dlg->widget().layer      = layer;
				m_link_to_copy_dlg->widget().layer_name = &r_doc_owner.document().currentLayer();
				m_link_to_copy_dlg->template eventHandler<ControlId::LinkToCopy>(*this);
			}
		}

		void onActivated(Tag<LayerAction::Rename>, Ui::MenuItem&)
		{
			if(auto layer = currentLayer(r_doc_owner.document()); layer != nullptr)
			{
				m_rename_dlg                      = std::make_unique<NameInputDlg>(r_dlg_owner,
                                                              "Rename current layer",
                                                              Ui::Box::Orientation::Horizontal,
                                                              "Name: ");
				m_rename_dlg->widget().layer      = layer;
				m_rename_dlg->widget().layer_name = &r_doc_owner.document().currentLayer();
				m_rename_dlg->template eventHandler<ControlId::Rename>(*this);
			}
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
			insertNewLayer(src.widget().inputField().content(), src.widget().layer->copiedLayer());
			m_copy_dlg.reset();
		}

		void dismiss(Tag<ControlId::Link>, NameInputDlg&) { m_link_dlg.reset(); }

		void confirmPositive(Tag<ControlId::Link>, NameInputDlg& src)
		{
			insertNewLayer(src.widget().inputField().content(), src.widget().layer->linkedLayer());
			m_link_dlg.reset();
		}

		void dismiss(Tag<ControlId::Copy>, NameInputDlg&) { m_copy_dlg.reset(); }

		template<class Dlg>
		void confirmPositive(Tag<ControlId::LinkToCopy>, Dlg& src)
		{
			r_doc_owner.documentModify([current_layer = src.widget().layer_name](auto& doc) {
				(void)doc.layersModify([current_layer](auto& layers) {
					layers[*current_layer]->convertToCopy();
					return true;
				});

				// Return false here because there are no changes to the ui
				// the ui
				return false;
			});

			m_link_to_copy_dlg.reset();
		}

		template<class Dlg>
		void confirmNegative(Tag<ControlId::LinkToCopy>, Dlg&)
		{
			m_link_to_copy_dlg.reset();
		}

		void confirmPositive(Tag<ControlId::Rename>, NameInputDlg& src)
		{
			r_doc_owner.documentModify([current_layer = src.widget().layer_name,
			                            new_name = src.widget().inputField().content()](auto& doc) {
				(void)doc.layersModify([current_layer, new_name](auto& layers) {
					layers.rename(*current_layer, new_name);
					return true;
				});
				return true;
			});

			insertNewLayer(src.widget().inputField().content(), src.widget().layer->linkedLayer());
			m_rename_dlg.reset();
		}

		void dismiss(Tag<ControlId::Rename>, NameInputDlg& src) { m_rename_dlg.reset(); }

		void onActivated(Tag<LayerActionNew::FromCurrentColor>, Ui::MenuItem&)
		{
			auto const size_max     = r_doc_owner.document().canvasSize();
			auto const size_default = Size2d{size_max.width() / 2, size_max.height() / 2};
			m_new_from_color_dlg    = std::make_unique<LayerCreatorDlg>(
                r_dlg_owner, "Create new layer from current color", size_default, size_max);
			m_new_from_color_dlg->eventHandler<ControlId::NewFromCurrentColor>(*this);
		}

		void onActivated(Tag<LayerActionNew::FromNoise>, Ui::MenuItem&)
		{
			auto const size_max     = r_doc_owner.document().canvasSize();
			auto const size_default = Size2d{size_max.width() / 2, size_max.height() / 2};
			m_new_from_noise        = std::make_unique<LayerCreatorDlg>(
                r_dlg_owner, "Create new layer from noise", size_default, size_max);
			m_new_from_noise->eventHandler<ControlId::NewFromNoise>(*this);
		}


		void confirmPositive(Tag<ControlId::NewFromCurrentColor>, LayerCreatorDlg& src)
		{
			auto layer_info = src.widget().value();
			if(!isSupported<Model::Pixel>(layer_info.size)) [[unlikely]]
				{
					// FIXME:
					//   "A layer of this size cannot be created. The number of bytes required to create a layer
					//   of "
					//  "this size exeeds the largest supported integer value."};
					return;
				}

			insertNewLayer(std::move(layer_info.name),
			               Model::Layer{layer_info.size, currentColor(r_doc_owner.document())});
			m_new_from_color_dlg.reset();
		}

		void dismiss(Tag<ControlId::NewFromCurrentColor>, LayerCreatorDlg&)
		{
			m_new_from_color_dlg.reset();
		}

		void confirmPositive(Tag<ControlId::NewFromNoise>, LayerCreatorDlg& src)
		{
			auto layer_info = src.widget().value();
			if(!isSupported<Model::Pixel>(layer_info.size)) [[unlikely]]
				{
					// FIXME:
					//   "A layer of this size cannot be created. The number of bytes required to create a layer
					//   of "
					//  "this size exeeds the largest supported integer value."};
					return;
				}

			Model::Image noise{layer_info.size};
			std::ranges::generate(noise.pixels(), [rng = m_rng]() mutable {
				std::uniform_real_distribution U{0.0f, 1.0f};
				return Model::Pixel{U(rng), U(rng), U(rng), U(rng)};
			});

			insertNewLayer(std::move(layer_info.name), Model::Layer{std::move(noise)});
			m_new_from_noise.reset();
		}

		void dismiss(Tag<ControlId::NewFromNoise>, LayerCreatorDlg&) { m_new_from_noise.reset(); }

	private:
		Ui::Container& r_dlg_owner;
		DocOwner& r_doc_owner;
		PolymorphicRng m_rng;

		std::unique_ptr<NameInputDlg> m_copy_dlg;
		std::unique_ptr<NameInputDlg> m_link_dlg;
		std::unique_ptr<ConfirmationDlg> m_link_to_copy_dlg;
		std::unique_ptr<NameInputDlg> m_rename_dlg;

		std::unique_ptr<LayerCreatorDlg> m_new_from_color_dlg;
		std::unique_ptr<LayerCreatorDlg> m_new_from_noise;

		void insertNewLayer(std::string&& layer_name, Model::Layer&& layer)
		{
			r_doc_owner.documentModify([&layer_name, &layer](auto& doc) noexcept {
				(void)doc.layersModify([layer_name, &layer](auto& layers) mutable noexcept {
					// FIXME: Unique name generator...
					layers.insert(std::make_pair(std::move(layer_name), std::move(layer)));
					return true;
				});
				doc.currentLayer(std::move(layer_name));
				return true;
			});
		}
	};
}

#endif