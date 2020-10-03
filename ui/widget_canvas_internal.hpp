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
#include <vector>
#include <algorithm>

#include <cassert>

GType widget_canvas_internal_get_type(void) G_GNUC_CONST;

namespace Texpainter::Ui
{
	class WidgetCanvasInternal: public GtkContainer
	{
		struct ChildInfo
		{
			WidgetCoordinates loc;
			bool fullsize;
		};

		using ChildrenMap = std::map<GtkWidget*, ChildInfo>;

	public:
		static WidgetCanvasInternal* create()
		{
			auto gobj = g_object_new(widget_canvas_internal_get_type(), nullptr);
			return new(gobj) Texpainter::Ui::WidgetCanvasInternal;
		}

		auto widgets() { return IterPair{std::begin(m_widgets), std::end(m_widgets)}; }

		auto children() { return IterPair{std::begin(m_children), std::end(m_children)}; }

		void remove(GtkWidget* widget)
		{
			auto i = std::ranges::find(m_widgets, widget);
			if(i == std::end(m_widgets)) { return; }
			m_widgets.erase(i);
			m_children.erase(widget);
		}

		void insert(GtkWidget* widget, WidgetCoordinates loc)
		{
			m_widgets.push_back(widget);
			m_children.insert(ChildrenMap::value_type{widget, {loc, false}});
			gtk_widget_set_parent(widget, GTK_WIDGET(this));
		}

		void insertFullsize(GtkWidget* widget)
		{
			m_widgets.push_back(widget);
			m_children.insert(ChildrenMap::value_type{widget, {WidgetCoordinates{0, 0}, true}});
			gtk_widget_set_parent(widget, GTK_WIDGET(this));
		}

		void move(GtkWidget* widget, WidgetCoordinates loc)
		{
			auto i = m_children.find(widget);
			assert(i != std::end(m_children));
			i->second = {loc, false};
			gtk_widget_queue_resize(GTK_WIDGET(this));
		}

		auto location(GtkWidget* widget) const
		{
			auto i = m_children.find(widget);
			assert(i != std::end(m_children));
			return i->second.loc;
		}

		void toFront(GtkWidget* widget)
		{
			auto i = std::ranges::find(m_widgets, widget);
			assert(i != std::end(m_widgets));
			std::swap(*i, m_widgets.back());
			gtk_widget_queue_draw(GTK_WIDGET(this));
		}

	private:
		WidgetCanvasInternal() {}
		std::vector<GtkWidget*> m_widgets;
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