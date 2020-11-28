//@	{"targets":[{"name":"brush_selector.hpp","type":"include"}]}

#ifndef TEXPAINTER_APP_BRUSHSELECTOR_HPP
#define TEXPAINTER_APP_BRUSHSELECTOR_HPP

#include "model_new/brushes.hpp"
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

		template<Model::BrushType type>
		BrushSelector& shape()
		{
			m_shapes.selected(static_cast<int>(type));
			return *this;
		}

		Model::BrushType shape() const
		{
			return static_cast<Model::BrushType>(m_shapes.selected());
		}

		double size() const { return linValue(m_size.value()); }

		BrushSelector& size(double value)
		{
			m_size.value(Ui::linValue(value));
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