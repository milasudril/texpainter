//@	{
//@	"targets":[{"name":"layer_menu_handler.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_LAYERMENUHANDLER_HPP
#define TEXPAINTER_LAYERMENUHANDLER_HPP

#include "./menu_layer.hpp"
#include "./size_input.hpp"
#include "./layer_creator.hpp"

#include "model/document.hpp"
#include "ui/dialog.hpp"

namespace Texpainter
{
	template<class DocOwner>
	class LayerMenuHandler
	{
		using NewFromCurrentColor = Ui::Dialog<LayerCreator>;

	public:
		enum class ControlId : int
		{
			NewFromCurrentColor
		};

		explicit LayerMenuHandler(Ui::Container& dialog_owner, DocOwner& doc_owner):
		   r_dlg_owner{dialog_owner},
		   r_doc_owner{doc_owner}
		{
		}

		template<LayerActionNew action>
		void onActivated(Ui::MenuItem& item)
		{
			onActivated(Tag<action>{}, item);
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

		void onActivated(Tag<LayerActionNew::FromCurrentColor>, Ui::MenuItem&)
		{
			if(r_doc_owner.hasDocument())
			{
				auto const size_max = r_doc_owner.document().canvasSize();
				auto const size_default = Size2d{size_max.width() / 2, size_max.height() / 2};
				m_new_from_color_dlg =
				   std::make_unique<NewFromCurrentColor>(r_dlg_owner, "New layer", size_default, size_max);
				m_new_from_color_dlg->eventHandler<ControlId::NewFromCurrentColor>(*this);
			}
			else
			{
				// TODO: Create new document and retrigger this action
			}
		}

		template<LayerActionNew action>
		void onActivated(Tag<action>, Ui::MenuItem& item)
		{
			printf("Todo: %d\n", static_cast<int>(action));
		}

		void confirmPositive(Tag<ControlId::NewFromCurrentColor>, NewFromCurrentColor& src)
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

		void dismiss(Tag<ControlId::NewFromCurrentColor>, NewFromCurrentColor&)
		{
			m_new_from_color_dlg.reset();
		}


	private:
		Ui::Container& r_dlg_owner;
		DocOwner& r_doc_owner;

		std::unique_ptr<NewFromCurrentColor> m_new_from_color_dlg;


		void insertNewLayer(std::string&& layer_name, Model::Layer&& layer)
		{
			r_doc_owner.documentModify([&layer_name, &layer ](auto& doc) noexcept {
				return doc.layersModify([ layer_name, &layer ](auto& layers) mutable noexcept {
					layers.insert(std::move(layer), std::move(layer_name), Model::LayerIndex{0});
					return true;
				});
				doc.currentLayer(std::move(layer_name));
			});
		}
	};
}

#endif