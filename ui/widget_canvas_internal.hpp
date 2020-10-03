//@	{
//@	  "targets":[{"name":"widget_canvas_internal.hpp", "type":"include", "pkgconfig_libs":["gtk+-3.0"]}]
//@	 ,"dependencies_extra":[{"ref":"widget_canvas_internal.o","rel":"implementation"}]
//@	 }

#ifndef TEXPAINTER_UI_WIDGETCANVASINTERNAL_HPP
#define TEXPAINTER_UI_WIDGETCANVASINTERNAL_HPP

#include "./widget_coordinates.hpp"

#include "utils/pair_iterator.hpp"
#include "utils/iter_pair.hpp"

#include <gtk/gtk.h>

#include <map>
#include <cassert>

GType widget_canvas_internal_get_type(void) G_GNUC_CONST;

namespace Texpainter::Ui
{
	class WidgetCanvasInternal: public GtkContainer
	{
		using ChildrenMap = std::map<GtkWidget*, WidgetCoordinates>;

	public:
		static WidgetCanvasInternal* create()
		{
			auto gobj = g_object_new(widget_canvas_internal_get_type(), nullptr);
			return new(gobj) Texpainter::Ui::WidgetCanvasInternal;
		}

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
			gtk_widget_set_parent(widget, GTK_WIDGET(this));
		}

		void move(GtkWidget* widget, WidgetCoordinates loc)
		{
			auto i = m_children.find(widget);
			assert(i != std::end(m_children));
			i->second = loc;
			gtk_widget_queue_resize(GTK_WIDGET(this));
		}

		auto location(GtkWidget* widget) const
		{
			auto i = m_children.find(widget);
			assert(i != std::end(m_children));
			return i->second;
		}

	private:
		WidgetCanvasInternal() {}
		ChildrenMap m_children;
	};


	inline WidgetCanvasInternal* asWidgetCanvasInternal(GtkWidget* widget)
	{
		return G_TYPE_CHECK_INSTANCE_CAST(
		    widget, widget_canvas_internal_get_type(), WidgetCanvasInternal);
	}

	inline WidgetCanvasInternal* asWidgetCanvasInternal(GtkContainer* widget)
	{
		return G_TYPE_CHECK_INSTANCE_CAST(
		    widget, widget_canvas_internal_get_type(), WidgetCanvasInternal);
	}

	inline WidgetCanvasInternal* asWidgetCanvasInternal(GObject* widget)
	{
		return G_TYPE_CHECK_INSTANCE_CAST(
		    widget, widget_canvas_internal_get_type(), WidgetCanvasInternal);
	}
}

#endif