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

		auto self = Texpainter::Ui::asWidgetCanvasInternal(widget);
		std::ranges::for_each(self->children(),
		                      [w = alloc->width, h = alloc->height](auto const& item) {
			                      GtkRequisition req;
			                      gtk_widget_get_preferred_size(item.first, &req, nullptr);

			                      GtkAllocation alloc{};
			                      if(item.second.fullsize)
			                      {
				                      alloc.width  = w;
				                      alloc.height = h;
			                      }
			                      else
			                      {
				                      alloc.width  = req.width;
				                      alloc.height = req.height;
				                      alloc.x      = static_cast<int>(item.second.loc.x());
				                      alloc.y      = static_cast<int>(item.second.loc.y());
			                      }
			                      gtk_widget_size_allocate(item.first, &alloc);
		                      });
	};

	widget_class->draw = [](GtkWidget* widget, cairo_t* cr) -> gboolean {
		auto self = Texpainter::Ui::asWidgetCanvasInternal(widget);
		std::ranges::for_each(self->widgets(), [self, cr](auto item) {
			gtk_container_propagate_draw(self, item, cr);
		});
		return FALSE;
	};

	klass->child_type = [](GtkContainer*) { return GTK_TYPE_WIDGET; };

	klass->add = [](GtkContainer*, GtkWidget*) { abort(); };

	klass->remove = [](GtkContainer* obj, GtkWidget* widget) {
		auto self = Texpainter::Ui::asWidgetCanvasInternal(obj);
		gtk_widget_unparent(widget);
		self->remove(widget);
	};

	klass->forall = [](GtkContainer* obj, gboolean, GtkCallback callback, gpointer data) {
		auto self = Texpainter::Ui::asWidgetCanvasInternal(obj);

		// Copy widget container in case callback calls remove
		auto widgets = self->widgets();
		std::vector<GtkWidget*> tmp{std::begin(widgets), std::end(widgets)};
		std::ranges::for_each(
		    tmp, [callback, user_data = data](auto item) { callback(item, user_data); });
	};

	auto g_object_class      = G_OBJECT_CLASS(klass);
	g_object_class->finalize = [](GObject* obj) {
		auto self = Texpainter::Ui::asWidgetCanvasInternal(obj);
		self->~WidgetCanvasInternal();
	};
}
