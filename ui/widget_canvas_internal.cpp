//@	{"targets":[{"name":"widget_canvas_internal.o", "type":"object", "pkgconfig_libs":["gtk+-3.0"]}]}

#include "./widget_canvas_internal.hpp"

#include <algorithm>

using Texpainter::Ui::WidgetCanvasInternal;

struct WidgetCanvasInternalClass: GtkContainerClass
{
};

#define volatile  // glib uses volatile in a non-conformant manner
G_DEFINE_TYPE(WidgetCanvasInternal, widget_canvas_internal, GTK_TYPE_CONTAINER);
#undef volatile

static void widget_canvas_internal_init(WidgetCanvasInternal* self)
{
	gtk_widget_set_has_window(GTK_WIDGET(self), FALSE);
}

static void widget_canvas_internal_class_init(WidgetCanvasInternalClass* klass)
{
	auto widget_class                 = GTK_WIDGET_CLASS(klass);
	widget_class->get_preferred_width = [](GtkWidget*, int* minimum, int* natural) {
		*minimum = 0;
		*natural = 0;
	};

	widget_class->get_preferred_height = [](GtkWidget*, int* minimum, int* natural) {
		*minimum = 0;
		*natural = 0;
	};

	widget_class->size_allocate = [](GtkWidget* widget, GtkAllocation* alloc) {
		gtk_widget_set_allocation(widget, alloc);
		if(gtk_widget_get_has_window(widget))
		{
			if(gtk_widget_get_realized(widget))
			{
				gdk_window_move_resize(
				    gtk_widget_get_window(widget), alloc->x, alloc->y, alloc->width, alloc->height);
			}
		}

		auto self = Texpainter::Ui::asWidgetCanvas(widget);
		std::ranges::for_each(self->m_widgets, [](auto const& item) {
			GtkRequisition req;
			gtk_widget_get_preferred_size(item.first, &req, nullptr);

			GtkAllocation alloc{};
			alloc.width  = req.width;
			alloc.height = req.height;
			alloc.x      = item.second.x();
			alloc.y      = item.second.y();
			gtk_widget_size_allocate(item.first, &alloc);
		});
	};

	widget_class->draw = [](GtkWidget* widget, cairo_t* cr) -> gboolean {
		auto self = Texpainter::Ui::asWidgetCanvas(widget);
		std::ranges::for_each(self->m_widgets, [self, cr](auto const& item) {
			gtk_container_propagate_draw(self, item.first, cr);
		});
		return FALSE;
	};

	klass->child_type = [](GtkContainer*) { return GTK_TYPE_WIDGET; };

	klass->add = [](GtkContainer*, GtkWidget*) { abort(); };

	klass->remove = [](GtkContainer* obj, GtkWidget* widget) {
		auto self = Texpainter::Ui::asWidgetCanvas(obj);
		gtk_widget_unparent(widget);
		self->m_widgets.erase(widget);
	};

	klass->forall = [](GtkContainer* obj, gboolean, GtkCallback callback, gpointer data) {
		auto self = Texpainter::Ui::asWidgetCanvas(obj);

		// Copy widget container in case callback calls remove
		std::vector<GtkWidget*> tmp;
		tmp.reserve(self->m_widgets.size());
		std::ranges::transform(
		    self->m_widgets, std::back_inserter(tmp), [](auto const& item) { return item.first; });
		std::ranges::for_each(
		    tmp, [callback, user_data = data](auto const& item) { callback(item, user_data); });
	};

	auto g_object_class      = G_OBJECT_CLASS(klass);
	g_object_class->finalize = [](GObject* obj) {
		auto self = Texpainter::Ui::asWidgetCanvas(obj);
		self->~WidgetCanvasInternal();
	};
}

WidgetCanvasInternal* widget_canvas_internal_new()
{
	auto gobj = g_object_new(widget_canvas_internal_get_type(), nullptr);
	return new(gobj) WidgetCanvasInternal;
}
