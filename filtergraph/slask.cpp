//@	{"targets":[{"name":"texpainter","type":"application", "pkgconfig_libs":["gtk+-3.0"]}]}

#include "utils/vec_t.hpp"

#include <gtk/gtk.h>

struct MoveData
{
	GtkWidget* current_widget;
	Texpainter::vec2_t origin;
};

gboolean button_press(GtkWidget* widget, GdkEvent* event, gpointer user_data)
{
	auto move_data = reinterpret_cast<MoveData*>(user_data);
	move_data->current_widget = gtk_widget_get_ancestor(widget, GTK_TYPE_BOX);

	auto layout = gtk_widget_get_ancestor(widget, GTK_TYPE_FIXED);

	int x{};
	int y{};
	gdk_window_get_origin (gtk_widget_get_window(layout),
                       &x,
                       &y);
	move_data->origin = Texpainter::vec2_t{static_cast<double>(x), static_cast<double>(y)};

	return FALSE;
}

gboolean button_relase(GtkWidget* widget, GdkEvent* event, gpointer user_data)
{
	reinterpret_cast<MoveData*>(user_data)->current_widget = nullptr;
	return FALSE;
}

gboolean mouse_move(GtkWidget* widget, GdkEvent* event, gpointer user_data)
{
	auto event_move = reinterpret_cast<GdkEventMotion const*>(event);

	auto move_data = reinterpret_cast<MoveData*>(user_data);
	if(move_data->current_widget != nullptr)
	{
		auto width  = gtk_widget_get_allocated_width(move_data->current_widget );
		auto height = gtk_widget_get_allocated_height(move_data->current_widget );

		auto dx = Texpainter::vec2_t{event_move->x_root, event_move->y_root}
			- move_data->origin
			- 0.5*Texpainter::vec2_t{static_cast<double>(width), static_cast<double>(height)};

		gtk_fixed_move(GTK_FIXED(widget),
		                move_data->current_widget,
		                dx[0],
		                dx[1]);
	}
	return TRUE;
}

gboolean draw_callback(GtkWidget* widget, cairo_t* cr, gpointer data)
{
	guint width, height;
	GdkRGBA color;
	GtkStyleContext* context;

	context = gtk_widget_get_style_context(widget);

	width  = gtk_widget_get_allocated_width(widget);
	height = gtk_widget_get_allocated_height(widget);

	gtk_render_background(context, cr, 0, 0, width, height);

	cairo_arc(cr, width / 2.0, height / 2.0, MIN(width, height) / 2.0, 0, 2 * G_PI);

	gtk_style_context_get_color(context, gtk_style_context_get_state(context), &color);
	gdk_cairo_set_source_rgba(cr, &color);

	cairo_fill(cr);

	return FALSE;
}

int main(int argc, char* argv[])
{
	gtk_disable_setlocale();
	gtk_init(&argc, &argv);

	auto mainwin = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
	gtk_window_set_default_size(mainwin, 800, 500);

	auto fixed = GTK_FIXED(gtk_fixed_new());
	gtk_container_add(GTK_CONTAINER(mainwin), GTK_WIDGET(fixed));

	MoveData move_data{};

	gtk_widget_set_events(GTK_WIDGET(fixed),
	                      GDK_POINTER_MOTION_MASK | GDK_BUTTON_PRESS_MASK
	                          | GDK_BUTTON_RELEASE_MASK);
	g_signal_connect(GTK_WIDGET(fixed), "motion-notify-event", G_CALLBACK(mouse_move), &move_data);

	auto box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
	gtk_fixed_put(fixed, GTK_WIDGET(box), 32, 32);

	auto drawing_area = gtk_drawing_area_new();
	printf("%p\n", drawing_area);
	gtk_widget_set_size_request(drawing_area, 100, 100);
	gtk_widget_set_events(GTK_WIDGET(drawing_area),
	                      GDK_POINTER_MOTION_MASK | GDK_BUTTON_PRESS_MASK
	                          | GDK_BUTTON_RELEASE_MASK);
	g_signal_connect(GTK_WIDGET(drawing_area), "draw", G_CALLBACK(draw_callback), NULL);
	g_signal_connect(
	    GTK_WIDGET(drawing_area), "button-press-event", G_CALLBACK(button_press), &move_data);
	g_signal_connect(
	    GTK_WIDGET(drawing_area), "button-release-event", G_CALLBACK(button_relase), &move_data);
	gtk_box_pack_start(box, GTK_WIDGET(drawing_area), FALSE, FALSE, 0);

	auto button = GTK_BUTTON(gtk_button_new_with_label("Hello"));
	gtk_box_pack_start(box, GTK_WIDGET(button), FALSE, FALSE, 0);

	gtk_widget_show_all(GTK_WIDGET(mainwin));
	gtk_main();

	return 0;
}