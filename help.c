#include <gtk/gtk.h>
#include "viewer.h"

/*********************          INIT_HELP            ********************/
void
init_help(viewer_app_t *app)
{
    GtkWindow	*win;

    win = GTK_WINDOW(gtk_builder_get_object(app->ui, "help_window"));
    gtk_window_set_deletable(win, false);
    app->help_window = win;
}

/*********************         DISPLAY_HELP          ********************/
void
display_help(viewer_app_t *app)
{
    gtk_widget_set_visible(GTK_WIDGET(app->help_window), true);
}

/*********************        ON_HELP_CLICKED        ********************/
G_MODULE_EXPORT void
on_help_clicked()
{
    extern viewer_app_t	*app;

    gtk_widget_set_visible(GTK_WIDGET(app->help_window), false);
}

// This prevents the window from being deleted when you click on the "X"
/*********************          CLOSE_HELP           ********************/
G_MODULE_EXPORT gboolean
close_help()
{
    return true;
}
