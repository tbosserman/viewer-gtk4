#include <gtk/gtk.h>
#include "viewer.h"

extern int		num_files;
extern char		*filenames[];
extern GtkWindow	*top_window;

extern void		show_file(int);

static GtkFileDialog	*dialog;

/*********************               X               ********************/

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

    list = gtk_file_dialog_open_multiple_finish(dialog, res, &err);
    if (err && err->code != 2)
    {
	fprintf(stderr, "dialog_cb: code=%d message='%s'\n", err->code,
	    err->message);
	g_error_free(err);
	return;
    }

    for (i = 0; i < num_files; ++i)
	g_free(filenames[i]);
    num_files = 0;

    for (i = 0; (ptr = g_list_model_get_item(list, i)) != NULL; ++i)
    {
	item = (GFile *)ptr;
	fname = g_file_get_path(item);
	filenames[num_files++] = fname;
	g_object_unref(item);
    }

    show_file(0);
}

/*********************         FILE_DIALOG           ********************/
void
file_dialog()
{
    gtk_file_dialog_open_multiple(dialog, top_window, NULL, dialog_cb, NULL);
}

/*********************       INIT_FILE_DIALOG        ********************/
void
init_file_dialog()
{
    dialog = gtk_file_dialog_new();
}
