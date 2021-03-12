//@	{
//@	 "targets":[{"name":"image_processor_selector.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_APP_IMAGEPROCESSORSELECTOR_HPP
#define TEXPAINTER_APP_IMAGEPROCESSORSELECTOR_HPP

#include "imgproc/image_processor_registry.hpp"

#include "ui/box.hpp"
#include "ui/labeled_input.hpp"
#include "ui/text_entry.hpp"
#include "ui/button.hpp"
#include "ui/listbox.hpp"
#include "ui/separator.hpp"
#include "ui/error_message_dialog.hpp"

namespace Texpainter::App
{
	class ImageProcessorSelector
	{
		enum class ControlId : int
		{
			Name,
			Listbox,
			ByCategory,
			ByName,
			Experimental,
			Deprecated
		};

	public:
		struct SortButtons
		{
			SortButtons(Ui::Container& owner)
			    : root{owner, Ui::Box::Orientation::Horizontal}
			    , by_category{root.homogenous(true).insertMode(
			                      Ui::Box::InsertMode{2, Ui::Box::Fill | Ui::Box::Expand}),
			                  "category"}
			    , by_name{root, "name"}
			{
			}
			Ui::Box root;
			Ui::Button by_category;
			Ui::Button by_name;
		};

		struct FilterButtons
		{
			FilterButtons(Ui::Container& owner)
			    : root{owner, Ui::Box::Orientation::Horizontal}
			    , experimental{root.homogenous(true).insertMode(
			                       Ui::Box::InsertMode{2, Ui::Box::Fill | Ui::Box::Expand}),
			                   "experimental"}
			    , deprecated{root, "deprecated"}
			{
			}
			Ui::Box root;
			Ui::Button experimental;
			Ui::Button deprecated;
		};

		explicit ImageProcessorSelector(Ui::Container& owner)
		    : m_root{owner, Ui::Box::Orientation::Vertical}
		    , m_search{m_root, Ui::Box::Orientation::Horizontal, "Search: "}
		    , m_listbox{m_root.insertMode(Ui::Box::InsertMode{2, Ui::Box::Fill | Ui::Box::Expand})}
		    , m_sort_buttons{m_root.insertMode(Ui::Box::InsertMode{2, 0}),
		                     Ui::Box::Orientation::Horizontal,
		                     "Order by "}
		    , m_filter_buttons{m_root.insertMode(Ui::Box::InsertMode{2, 0}),
		                       Ui::Box::Orientation::Horizontal,
		                       "Show "}
		    , m_separator{m_root}

		{
			set_by_category();
			m_search.inputField().eventHandler<ControlId::Name>(*this).continuousUpdate(true);
			m_sort_buttons.inputField().by_category.eventHandler<ControlId::ByCategory>(*this);
			m_sort_buttons.inputField().by_name.eventHandler<ControlId::ByName>(*this);
			m_filter_buttons.inputField().experimental.eventHandler<ControlId::Experimental>(*this);
			m_filter_buttons.inputField().deprecated.eventHandler<ControlId::Deprecated>(*this);
		}

		char const* value() const { return m_listbox.get(m_listbox.selected()); }

		template<auto>
		void onChanged(Ui::TextEntry& src)
		{
			select(src.content());
		}

		template<auto, class T>
		void handleException(char const* message, T&)
		{
			m_err_box.show(m_root, "Texpainter: Adding image processor", message);
		}

		template<auto>
		void onClicked(Ui::Button& src);

		template<auto id, class EventHandler>
		ImageProcessorSelector& eventHandler(EventHandler& eh)
		{
			m_listbox.template eventHandler<id>(eh);
			return *this;
		}

	private:
		Ui::Box m_root;
		Ui::LabeledInput<Ui::TextEntry> m_search;
		Ui::Listbox m_listbox;
		Ui::LabeledInput<SortButtons> m_sort_buttons;
		Ui::LabeledInput<FilterButtons> m_filter_buttons;
		Ui::Separator m_separator;
		Ui::ErrorMessageDialog m_err_box;
		std::vector<ImageProcessorRegistry::ImageProcessorInfo> m_filtered_procs;

		void set_by_category()
		{
			m_sort_buttons.inputField().by_category.state(true);
			m_sort_buttons.inputField().by_name.state(false);
			reset_list(ImageProcessorRegistry::imageProcessorsByCategory());
		}

		void set_by_name()
		{
			m_sort_buttons.inputField().by_category.state(false);
			m_sort_buttons.inputField().by_name.state(true);
			reset_list(ImageProcessorRegistry::imageProcessorsByName());
		}

		void reset_list(std::span<ImageProcessorRegistry::ImageProcessorInfo const> imgprocs)
		{
			auto selected = m_listbox.get(m_listbox.selected());

			m_filtered_procs.clear();
			std::ranges::copy_if(
			    imgprocs,
			    std::back_inserter(m_filtered_procs),
			    [show_experimental = m_filter_buttons.inputField().experimental.state(),
			     show_deprecated   = m_filter_buttons.inputField().deprecated.state()](auto item) {
				    return item.release_state == FilterGraph::ImgProcReleaseState::Stable
				           || (show_experimental && show_deprecated)
				           || (show_experimental
				               && item.release_state
				                      == FilterGraph::ImgProcReleaseState::Experimental)
				           || (show_deprecated
				               && item.release_state
				                      == FilterGraph::ImgProcReleaseState::Deprecated);
			    });

			std::string sel{selected != nullptr ? selected : ""};
			m_listbox.clear();

			std::ranges::for_each(m_filtered_procs,
			                      [&list = m_listbox](auto item) { list.append(item.name); });
			m_listbox.update();

			if(sel.size() != 0) { select(sel.c_str()); }
		}

		void select(char const* val)
		{
			auto i = std::ranges::find_if(m_filtered_procs, [val](auto const& item) {
				return strcasestr(item.name, val) != nullptr;
			});
			if(i == std::end(m_filtered_procs)) { return; }

			auto const index = static_cast<int>(i - std::begin(m_filtered_procs));
			m_listbox.scrollIntoView(index).selected(index);
		}
	};

	template<>
	inline void ImageProcessorSelector::onClicked<ImageProcessorSelector::ControlId::ByCategory>(
	    Ui::Button&)
	{
		set_by_category();
	}

	template<>
	inline void ImageProcessorSelector::onClicked<ImageProcessorSelector::ControlId::ByName>(
	    Ui::Button&)
	{
		set_by_name();
	}

	template<>
	inline void ImageProcessorSelector::onClicked<ImageProcessorSelector::ControlId::Deprecated>(
	    Ui::Button&)
	{
		if(m_sort_buttons.inputField().by_name.state()) { set_by_name(); }
		else
		{
			set_by_category();
		}
	}

	template<>
	inline void ImageProcessorSelector::onClicked<ImageProcessorSelector::ControlId::Experimental>(
	    Ui::Button&)
	{
		if(m_sort_buttons.inputField().by_name.state()) { set_by_name(); }
		else
		{
			set_by_category();
		}
	}
}

#endif
