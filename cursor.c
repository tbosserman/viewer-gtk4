#include <gtk/gtk.h>

#define CURSOR_PATH	"/usr/share/icons/breeze_cursors/cursors_scalable"
#define CURSOR_FILE	CURSOR_PATH "/crosshair/crosshair.svg"

static GdkCursor	*cursor = NULL;
static GdkPixbuf	*cursor_pixbuf = NULL;
static double		cursor_scale = 1.0;
static int		cursor_width, cursor_height;
extern GtkBuilder	*ui_xml;

/********************                X               ********************/

/********************           SET_CURSOR           ********************/
void
set_cursor(double scale_incr)
{
    GdkTexture	*texture;
    GdkPixbuf	*pixbuf;
    GError	*error;
    GtkWidget	*widget;
    float	scaled_width, scaled_height;
    int		hot_x, hot_y;

    error = NULL;
    if (cursor_pixbuf == NULL)
    {
	cursor_pixbuf = gdk_pixbuf_new_from_file(CURSOR_FILE, &error);
	if (error)
	{
	    printf("error #%d loading texture: %s\n", error->code, error->message);
	    g_object_unref(error);
	    return;
	}
	cursor_width = gdk_pixbuf_get_width(cursor_pixbuf);
	cursor_height = gdk_pixbuf_get_height(cursor_pixbuf);
    }

    cursor_scale += scale_incr;
    scaled_width = (float)cursor_width * cursor_scale;
    scaled_height = (float)cursor_height * cursor_scale;
    printf("Cursor is %d x %d pixels\n", cursor_width, cursor_height);
    printf("Scaled: %.2f x %.2f\n", scaled_width, scaled_height);

    hot_x = (scaled_width + 0.5) / 2;
    hot_y = (scaled_height + 0.5) / 2;
    if (cursor)
	g_object_unref(cursor);

    // Now actually scale the cursor image. Conversion between pixbuf
    // and texture is "deprecated", but I can't find the "modern" way
    // to accomplish the same thing.
    pixbuf = gdk_pixbuf_scale_simple(cursor_pixbuf, scaled_width,
	scaled_height, GDK_INTERP_BILINEAR);
G_GNUC_BEGIN_IGNORE_DEPRECATIONS
    texture = gdk_texture_new_for_pixbuf (pixbuf);
G_GNUC_END_IGNORE_DEPRECATIONS

    // Now we actually create the cursor and assign it to the image window.
    cursor = gdk_cursor_new_from_texture(texture, hot_x, hot_y, NULL);
    g_object_unref(texture);
    
    widget = (GtkWidget *)gtk_builder_get_object(ui_xml, "image_window");
    gtk_widget_set_cursor(widget, cursor);
}

/********************    GET_CURRENT_CURSOR_SCALE    ********************/
double
get_current_cursor_scale()
{
    return cursor_scale;
}
