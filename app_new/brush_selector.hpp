//@	{"targets":[{"name":"brush_selector.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_BRUSHSELECTOR_HPP
#define TEXPAINTER_APP_BRUSHSELECTOR_HPP

#include "model_new/brush.hpp"
#include "ui/combobox.hpp"
#include "ui/slider.hpp"
#include "ui/box.hpp"

namespace Texpainter::App
{
	class BrushSelector
	{
	public:
		explicit BrushSelector(Ui::Container& owner)
		    : m_root{owner, Ui::Box::Orientation::Horizontal}
		    , m_shapes{m_root}
		    , m_size{m_root.insertMode(Ui::Box::InsertMode{0, Ui::Box::Fill | Ui::Box::Expand}),
		             false}
		{
			forEachEnumItem<Model::BrushType>([&shapes = m_shapes]<class T>(T) {
				shapes.append(Model::BrushTraits<T::value>::displayName());
			});
		}

		Model::BrushInfo brush() const
		{
			return Model::BrushInfo{static_cast<float>(linValue(m_size.value())),
			                        static_cast<Model::BrushType>(m_shapes.selected())};
		}

		BrushSelector& brush(Model::BrushInfo brush)
		{
			m_shapes.selected(static_cast<int>(brush.shape()));
			m_size.value(Ui::linValue(brush.radius()));
			return *this;
		}

		template<auto id, class EventHandler>
		BrushSelector& eventHandler(EventHandler& eh)
		{
			m_shapes.eventHandler<id>(eh);
			m_size.eventHandler<id>(eh);
			return *this;
		}

	private:
		Ui::Box m_root;
		Ui::Combobox m_shapes;
		Ui::Slider m_size;
	};
}

#endif