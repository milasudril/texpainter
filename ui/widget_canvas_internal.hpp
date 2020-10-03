//@	{"targets":[{"name":"widget_canvas_internal.hpp", "type":"include", "pkgconfig_libs":["gtk+-3.0"]}]}

#ifndef TEXPAINTER_UI_WIDGETCANVASINTERNAL_HPP
#define TEXPAINTER_UI_WIDGETCANVASINTERNAL_HPP

#include "./widget_coordinates.hpp"

#include "utils/pair_iterator.hpp"
#include "utils/iter_pair.hpp"

#include <gtk/gtk.h>

#include <map>

GType widget_canvas_internal_get_type(void) G_GNUC_CONST;

namespace Texpainter::Ui
{
	class WidgetCanvasInternal: public GtkContainer
	{
		using ChildrenMap = std::map<GtkWidget*, WidgetCoordinates>;

	public:
		auto widgets()
		{
			return IterPair{PairFirstIterator{std::begin(m_children)},
			                PairFirstIterator{std::end(m_children)}};
		}

		auto locations() const
		{
			return IterPair{PairSecondIterator{std::begin(m_children)},
			                PairSecondIterator{std::end(m_children)}};
		}

		auto children() { return IterPair{std::begin(m_children), std::end(m_children)}; }

		void remove(GtkWidget* widget) { m_children.erase(widget); }

		void insert(GtkWidget* widget, WidgetCoordinates loc)
		{
			m_children.insert(ChildrenMap::value_type{widget, loc});
		}

	private:
		ChildrenMap m_children;
	};

	WidgetCanvasInternal* widget_canvas_internal_new();

	inline WidgetCanvasInternal* asWidgetCanvas(GtkWidget* widget)
	{
		return G_TYPE_CHECK_INSTANCE_CAST(
		    widget, widget_canvas_internal_get_type(), WidgetCanvasInternal);
	}

	inline WidgetCanvasInternal* asWidgetCanvas(GtkContainer* widget)
	{
		return G_TYPE_CHECK_INSTANCE_CAST(
		    widget, widget_canvas_internal_get_type(), WidgetCanvasInternal);
	}

	inline WidgetCanvasInternal* asWidgetCanvas(GObject* widget)
	{
		return G_TYPE_CHECK_INSTANCE_CAST(
		    widget, widget_canvas_internal_get_type(), WidgetCanvasInternal);
	}
}

#endif