//@	{
//@  "targets":[{"name":"labeled_input.hpp","type":"include"}]
//@	}

#ifndef TEXPAINTER_UI_LABELEDINPUT_HPP
#define TEXPAINTER_UI_LABELEDINPUT_HPP

#include "./box.hpp"
#include "./label.hpp"

namespace Texpainter::Ui
{
	template<class Widget, class LabelType = Label, bool Reversed = false>
	class LabeledInput
	{
	public:
		template<class... Args>
		explicit LabeledInput(Container& container,
		                      Box::Orientation orientation,
		                      const char* label,
		                      Args&&... args) requires(!Reversed)
		    : m_box{container, orientation}
		    , m_widgets{LabelType{m_box, label},
		                std::make_unique<Widget>(
		                    m_box.insertMode(Box::InsertMode{0, Box::Expand | Box::Fill}),
		                    std::forward<Args>(args)...)}
		{
		}

		template<class... Args>
		explicit LabeledInput(Container& container,
		                      Box::Orientation orientation,
		                      const char* label,
		                      Args&&... args) requires(Reversed)
		    : m_box{container, orientation}
		    , m_widgets{
		          std::make_unique<Widget>(m_box, std::forward<Args>(args)...),
		          LabelType{m_box.insertMode(Box::InsertMode{0, Box::Expand | Box::Fill}), label}}
		{
		}

		LabelType& label() { return const_cast<LabelType&>(std::as_const(*this).label()); }

		LabelType const& label() const
		{
			if constexpr(Reversed) { return m_widgets.second; }
			else
			{
				return m_widgets.first;
			}
		}

		Widget& inputField() { return const_cast<Widget&>(std::as_const(*this).inputField()); }

		Widget const& inputField() const
		{
			if constexpr(Reversed) { return *m_widgets.first; }
			else
			{
				return *m_widgets.second;
			}
		}


	private:
		Box m_box;

		using WidgetPtr = std::unique_ptr<Widget>;
		std::conditional_t<Reversed,
		                   std::pair<WidgetPtr, LabelType>,
		                   std::pair<LabelType, WidgetPtr>>
		    m_widgets;
	};

	template<class Widget, class LabelType = Label>
	using ReversedLabeledInput = LabeledInput<Widget, LabelType, true>;
}

#endif