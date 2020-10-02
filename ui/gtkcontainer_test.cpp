//@	{"targets":[{"name":"gtkcontainer_test", "type":"application", "pkgconfig_libs":["gtk+-3.0"]}]}

#include "./widget_coordinates.hpp"

#include <gtk/gtk.h>

#include <new>
#include <map>
#include <vector>
#include <algorithm>

struct MyContainer: GtkContainer
{
	std::map<GtkWidget*, Texpainter::Ui::WidgetCoordinates> m_children;
};

void addWidget(MyContainer& obj, GtkWidget* widget, Texpainter::Ui::WidgetCoordinates loc)
{
	obj.m_children.insert(std::pair{widget, loc});
	gtk_widget_set_parent(widget, GTK_WIDGET(&obj));
}

struct MyContainerClass: GtkContainerClass
{
};

#define volatile  // glib use volatile in a non-conformant manner
G_DEFINE_TYPE(MyContainer, my_container, GTK_TYPE_CONTAINER);
#undef volatile

MyContainer* my_container_new()
{
	auto obj = g_object_new(my_container_get_type(), nullptr);
	auto ret = G_TYPE_CHECK_INSTANCE_CAST(obj, my_container_get_type(), MyContainer);
	return new(ret) MyContainer;
}

void my_container_init(MyContainer* self) { gtk_widget_set_has_window(GTK_WIDGET(self), FALSE); }

void my_container_class_init(MyContainerClass* klass)
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

		auto self = G_TYPE_CHECK_INSTANCE_CAST(widget, my_container_get_type(), MyContainer);
		std::ranges::for_each(self->m_children, [](auto const& item) {
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
		auto self = G_TYPE_CHECK_INSTANCE_CAST(widget, my_container_get_type(), MyContainer);
		std::ranges::for_each(self->m_children, [self, cr](auto const& item) {
			gtk_container_propagate_draw(self, item.first, cr);
		});
		return FALSE;
	};

	klass->child_type = [](GtkContainer*) { return GTK_TYPE_WIDGET; };

	klass->add = [](GtkContainer*, GtkWidget*) { abort(); };

	klass->remove = [](GtkContainer* obj, GtkWidget* widget) {
		auto self = G_TYPE_CHECK_INSTANCE_CAST(obj, my_container_get_type(), MyContainer);
		gtk_widget_unparent(widget);
		self->m_children.erase(widget);
	};

	klass->forall = [](GtkContainer* obj, gboolean, GtkCallback callback, gpointer data) {
		auto self = G_TYPE_CHECK_INSTANCE_CAST(obj, my_container_get_type(), MyContainer);

		// Copy widget container in case callback calls remove
		std::vector<GtkWidget*> tmp;
		tmp.reserve(self->m_children.size());
		std::ranges::transform(
		    self->m_children, std::back_inserter(tmp), [](auto const& item) { return item.first; });
		std::ranges::for_each(
		    tmp, [callback, user_data = data](auto const& item) { callback(item, user_data); });
	};

	auto g_object_class      = G_OBJECT_CLASS(klass);
	g_object_class->finalize = [](GObject* obj) {
		auto self = G_TYPE_CHECK_INSTANCE_CAST(obj, my_container_get_type(), MyContainer);
		self->~MyContainer();
	};
}


int main()
{
	gtk_init(nullptr, nullptr);

	auto mainwin   = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	auto container = my_container_new();
	gtk_container_add(GTK_CONTAINER(mainwin), GTK_WIDGET(container));
	g_signal_connect(mainwin,
	                 "delete-event",
	                 G_CALLBACK(+[](GtkWidget*, GdkEvent*, gpointer) {
		                 gtk_main_quit();
		                 return FALSE;
	                 }),
	                 nullptr);

	auto button = gtk_button_new_with_label("Hej");
	addWidget(*container, button, Texpainter::Ui::WidgetCoordinates{40.0, 60.0});
	gtk_widget_show_all(mainwin);
	gtk_main();

	return 0;
}