#include <gtk/gtk.h>

#define CURSOR_PATH	"/usr/share/icons/breeze_cursors/cursors_scalable"
#define CURSOR_FILE	CURSOR_PATH "/crosshair/crosshair.svg"

static GdkCursor	*cursor = NULL;

extern GtkBuilder	*ui_xml;

/********************                X               ********************/
void
set_cursor(double scale)
{
    GdkTexture	*texture;
    GError	*error;
    GtkWidget	*widget;
    int		width, height, hot_x, hot_y;

    error = NULL;
    texture = gdk_texture_new_from_filename(CURSOR_FILE, &error);
    if (error)
    {
	printf("error #%d loading texture: %s\n", error->code, error->message);
	g_object_unref(error);
	return;
    }

    width = gdk_texture_get_height(texture);
    height = gdk_texture_get_height(texture);
    printf("Cursor is %d x %d pixels\n", width, height);

    hot_x = width / 2;
    hot_y = height / 2;
    if (cursor)
	g_object_unref(cursor);
    cursor = gdk_cursor_new_from_texture(texture, hot_x, hot_y, NULL);
    g_object_unref(texture);
    
    widget = (GtkWidget *)gtk_builder_get_object(ui_xml, "image_window");
    gtk_widget_set_cursor(widget, cursor);
}
