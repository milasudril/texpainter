//@	{"targets":[{"name":"texpainter","type":"application", "pkgconfig_libs":["gtk+-3.0"]}]}


#include "./document_editor.hpp"
#include <gtk/gtk.h>

int main(int argc, char* argv[])
{
	gtk_disable_setlocale();
	gtk_init(&argc, &argv);

	Texpainter::App::DocumentEditor editor;

	gtk_main();
}
