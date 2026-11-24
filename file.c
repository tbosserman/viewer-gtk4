#include <gtk/gtk.h>
#include "viewer.h"

extern viewer_app_t	*app;

/*********************          DIALOG_CB            ********************/
void
dialog_cb(GObject *obj, GAsyncResult *res, gpointer data)
{
    char	*fname;
    guint	i;
    gpointer	ptr;
    GFile	*item;
    GListModel	*list;
    GError	*err = NULL;

    list = gtk_file_dialog_open_multiple_finish(app->file_dialog, res, &err);
    if (err)
    {
	if (err->code != 2)
	    fprintf(stderr, "dialog_cb: code=%d message='%s'\n", err->code,
		err->message);
	g_error_free(err);
	return;
    }

    for (i = 0; i < app->num_files; ++i)
	g_free(app->filenames[i]);
    app->num_files = 0;

    for (i = 0; (ptr = g_list_model_get_item(list, i)) != NULL; ++i)
    {
	item = (GFile *)ptr;
	fname = g_file_get_path(item);
	app->filenames[app->num_files++] = fname;
	g_object_unref(item);
    }
    g_object_unref(list);

    show_file(app, 0);
}

/*********************         FILE_DIALOG           ********************/
void
file_dialog(viewer_app_t *app)
{
    gtk_file_dialog_open_multiple(app->file_dialog, app->top_window, NULL,
	dialog_cb, NULL);
}

/*********************       INIT_FILE_DIALOG        ********************/
GtkFileDialog *
init_file_dialog()
{
    return(gtk_file_dialog_new());
}
