#include <gtk/gtk.h>
#include <glycin.h>
#include <glycin-gtk4.h>
#include <math.h>
#include "viewer.h"

extern GtkBuilder	*ui_xml;
extern void		alert(const char *fmt, ...);

int			filenum, num_files;
char			*filenames[MAX_FILES];

static float		scale_factor = 1.0;
static int		can_shrink = TRUE;
static GdkTexture	*texture = NULL;
static GlyFrame		*frame = NULL;
static GdkPaintable	*paintable = NULL;
static uint32_t		image_width, image_height;

/********************        REMOVE_FROM_LIST        ********************/
void
remove_from_list(int num)
{
    int 	i;

    for (i = num; i < num_files-1; ++i)
	filenames[i] = filenames[i+1];
    --num_files;
}

/********************           SHOW_FILE            ********************/
void
show_file(int num)
{
    GtkWindow		*window;
    GtkPicture		*gtk_pic;
    GFile		*gfile;
    GlyLoader		*loader;
    GlyImage		*image;
    GError		*error;
    GtkSnapshot		*snap;
    graphene_rect_t	rect;

    if (texture)
	g_object_unref((GObject *)texture);
    if (frame)
	g_object_unref((GObject *)frame);
    if (paintable)
	g_object_unref((GObject *)paintable);
    texture = NULL;
    image = NULL;
    frame = NULL;
    paintable = NULL;

    error = NULL;
    gfile = g_file_new_for_path (filenames[num]);
    loader = gly_loader_new(gfile);
    image = gly_loader_load(loader, &error);
    g_object_unref((GObject *)loader);
    snap = gtk_snapshot_new();
    if (!error)
	frame = gly_image_next_frame(image, &error);
    if (error)
    {
	alert(error->message);
	g_clear_error(&error);
	g_object_unref((GObject *)gfile);
	remove_from_list(num);
	return;
    }
    g_object_unref((GObject *)image);

    image_width = gly_frame_get_width(frame);
    image_height = gly_frame_get_height(frame);
    rect = GRAPHENE_RECT_INIT(0.0, 0.0, (float)image_width, (float)image_height);
    texture = gly_gtk_frame_get_texture(frame);
    gtk_snapshot_append_texture(snap, texture, &rect);
    paintable = gtk_snapshot_free_to_paintable(snap, &rect.size);
    gtk_pic = (GtkPicture *)gtk_builder_get_object(ui_xml, "image_window");
    gtk_picture_set_paintable(gtk_pic, paintable);
    
    window = (GtkWindow *)gtk_builder_get_object(ui_xml, "top_window");
    gtk_window_set_title(window, filenames[num]);

    g_object_unref((GObject *)gfile);
    filenum = num;
}

/********************          TOGGLE_FILL           ********************/
void
toggle_fill()
{
    GtkPicture	*pic;

    pic = (GtkPicture *)gtk_builder_get_object(ui_xml, "image_window");
    can_shrink = !can_shrink;
    gtk_picture_set_can_shrink(pic, can_shrink);
}

/************************************************************************
 ********************             SCALE              ********************
 ************************************************************************/
void
scale(int direction)
{
    GtkWidget		*widget;
    GtkSnapshot		*snap;
    GtkPicture		*gtk_pic;
    int			widget_width, widget_height;
    float		xfactor, yfactor, width, height, min_factor;
    graphene_rect_t	rect;

    widget = (GtkWidget *)gtk_builder_get_object(ui_xml, "scroll_window");
    widget_width = gtk_widget_get_width(widget);
    widget_height = gtk_widget_get_height(widget);
    xfactor = (float)widget_width / (float)image_width;
    yfactor = (float)widget_height / (float)image_height;
    min_factor = fmin(xfactor, yfactor);

    if (can_shrink)
    {
	toggle_fill();
	scale_factor = min_factor;
    }

    switch(direction)
    {
	case SCALE_UP:
	    if ((scale_factor += 0.1) > 1.0)
		scale_factor = 1.0;
	    break;
	case SCALE_DOWN:
	    if ((scale_factor -= 0.1) < min_factor)
		scale_factor = min_factor;
	    break;
    }

    if (paintable)
	g_object_unref((GObject *)paintable);

    snap = gtk_snapshot_new();
    width = image_width * scale_factor;
    height = image_height * scale_factor;
    rect = GRAPHENE_RECT_INIT(0.0, 0.0, width, height);
    gtk_snapshot_append_texture(snap, texture, &rect);
    paintable = gtk_snapshot_free_to_paintable(snap, &rect.size);
    gtk_pic = (GtkPicture *)gtk_builder_get_object(ui_xml, "image_window");
    gtk_picture_set_paintable(gtk_pic, paintable);
}
