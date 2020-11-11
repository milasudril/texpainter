//@	 {"targets":[{"name":"filename_select.o","type":"object","pkgconfig_libs":["gtk+-3.0"]}]}

#include "./filename_select.hpp"
#include "./container.hpp"
#include <gtk/gtk.h>

bool Texpainter::Ui::filenameSelect(Container const& cnt,
                                    std::filesystem::path const& working_dir,
                                    std::filesystem::path& filename_in,
                                    FilenameSelectMode mode,
                                    FilenameSelectFilter cb,
                                    void* cb_obj,
                                    char const* filter_name)
{
	auto action = mode == FilenameSelectMode::Open ? GTK_FILE_CHOOSER_ACTION_OPEN
	                                               : GTK_FILE_CHOOSER_ACTION_SAVE;

	char const* text_ok = mode == FilenameSelectMode::Open ? "Open" : "Save";
	char const* text_title =
	    mode == FilenameSelectMode::Open ? "Select file to open" : "Choose filename";

	auto parent = cnt.toplevel();
	auto dlg    = gtk_file_chooser_dialog_new(text_title,
                                           GTK_WINDOW(parent),
                                           action,
                                           "Cancel",
                                           GTK_RESPONSE_CANCEL,
                                           text_ok,
                                           GTK_RESPONSE_ACCEPT,
                                           nullptr);
	gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dlg),
	                                               mode == FilenameSelectMode::Save);
	if(!filename_in.empty())
	{
		auto filename_tot = working_dir / filename_in;
		gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dlg), filename_tot.c_str());
	}
	else
	{
		gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dlg), working_dir.c_str());
	}
	gtk_file_chooser_add_shortcut_folder(GTK_FILE_CHOOSER(dlg), working_dir.c_str(), nullptr);

	auto filter = gtk_file_filter_new();
	gtk_file_filter_set_name(filter, filter_name);
	std::pair<FilenameSelectFilter, void*> cb_obj_gtk{cb, cb_obj};
	auto cb_gtk = [](GtkFileFilterInfo const* filter_info, gpointer data) {
		auto obj = reinterpret_cast<std::pair<FilenameSelectFilter, void*>*>(data);
		if(obj->first(obj->second, filter_info->filename)) { return 1; }
		return 0;
	};

	gtk_file_filter_add_custom(filter, GTK_FILE_FILTER_FILENAME, cb_gtk, &cb_obj_gtk, nullptr);
	gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(dlg), filter);

	if(gtk_dialog_run(GTK_DIALOG(dlg)) == GTK_RESPONSE_ACCEPT)
	{
		auto filename_out = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dlg));
		try
		{
			filename_in = filename_out;
		}
		catch(...)
		{
			gtk_widget_destroy(dlg);
			g_free(filename_out);
			throw;
		}
		gtk_widget_destroy(dlg);
		g_free(filename_out);
		return 1;
	}
	gtk_widget_destroy(dlg);
	return 0;
}