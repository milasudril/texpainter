//@	{
//@	 "targets":[{"name":"image_processor_selector.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_APP_IMAGEPROCESSORSELECTOR_HPP
#define TEXPAINTER_APP_IMAGEPROCESSORSELECTOR_HPP

#include "ui/box.hpp"
#include "ui/labeled_input.hpp"
#include "ui/text_entry.hpp"
#include "ui/button.hpp"
#include "ui/listbox.hpp"
#include "ui/separator.hpp"
#include "imgproc/image_processor_registry.hpp"

namespace Texpainter::App
{
	class ImageProcessorSelector
	{
		enum class ControlId : int
		{
			Name,
			Listbox,
			ByCategory,
			ByName
		};

	public:
		explicit ImageProcessorSelector(Ui::Container& owner)
		    : m_root{owner, Ui::Box::Orientation::Vertical}
		    , m_search{m_root, Ui::Box::Orientation::Horizontal, "Search: "}
		    , m_listbox{m_root}
		    , m_sort_buttons{m_root, Ui::Box::Orientation::Horizontal}
		    , m_by_category{m_sort_buttons.homogenous(true).insertMode(
		                        Ui::Box::InsertMode{2, Ui::Box::Fill | Ui::Box::Expand}),
		                    "By category"}
		    , m_by_name{m_sort_buttons, "By name"}
		    , m_separator{m_root}

		{
			set_by_category();
			m_search.inputField().eventHandler<ControlId::Name>(*this).continuousUpdate(true);
			m_by_category.eventHandler<ControlId::ByCategory>(*this);
			m_by_name.eventHandler<ControlId::ByName>(*this);
		}

		char const* value() const { return m_listbox.get(m_listbox.selected()); }

		template<auto>
		void onChanged(Ui::TextEntry& src)
		{
			select(src.content());
		}

		template<auto, class T>
		void handleException(char const*, T&)
		{
		}

		template<auto>
		void onClicked(Ui::Button& src);

	private:
		Ui::Box m_root;
		Ui::LabeledInput<Ui::TextEntry> m_search;
		Ui::Listbox m_listbox;
		Ui::Box m_sort_buttons;
		Ui::Button m_by_category;
		Ui::Button m_by_name;
		Ui::Separator m_separator;

		void set_by_category()
		{
			m_by_category.state(true);
			m_by_name.state(false);
			reset_list(ImageProcessorRegistry::imageProcessorsByCategory());
		}

		void set_by_name()
		{
			m_by_category.state(false);
			m_by_name.state(true);
			reset_list(ImageProcessorRegistry::imageProcessorsByName());
		}

		void reset_list(std::span<ImageProcessorRegistry::ImageProcessorInfo const> imgprocs)
		{
			auto selected = m_listbox.get(m_listbox.selected());
			std::string sel{selected != nullptr ? selected : ""};
			m_listbox.clear();
			std::ranges::for_each(imgprocs, [&list = m_listbox](auto item) {
				list.append(item.name);
			});
			m_listbox.update();

			if(sel.size() != 0) { select(sel.c_str()); }
		}

		void select(char const* val)
		{
			auto const items = m_by_category.state()
			                       ? ImageProcessorRegistry::imageProcessorsByCategory()
			                       : ImageProcessorRegistry::imageProcessorsByName();

			auto i = std::ranges::find_if(
			    items, [val](auto const& item) { return strstr(item.name, val) != nullptr; });
			if(i == std::end(items)) { return; }

			auto const index = i - std::begin(items);
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
}

#endif
