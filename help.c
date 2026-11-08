#include <gtk/gtk.h>
#include "viewer.h"

#define HELP_FILE	"Help.html"

extern GtkBuilder	*ui_xml;

static GtkWindow	*help_window;

/*********************               X               ********************/

/*********************          INIT_HELP            ********************/
void
init_help()
{
    help_window = (GtkWindow *)gtk_builder_get_object(ui_xml, "help_window");
    gtk_window_set_deletable(help_window, false);
}

/*********************         DISPLAY_HELP          ********************/
void
display_help()
{
    gtk_widget_set_visible(GTK_WIDGET(help_window), 1);
}

/*********************        ON_HELP_CLICKED        ********************/
G_MODULE_EXPORT void
on_help_clicked()
{
    gtk_widget_set_visible(GTK_WIDGET(help_window), 0);
}

// This prevents the window from being deleted when you click on the "X"
/*********************          CLOSE_HELP           ********************/
G_MODULE_EXPORT gboolean
close_help()
{
    return true;
}
