#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <glycin.h>
#include <glycin-gtk4.h>
#include "viewer.h"

extern GtkBuilder	*ui_xml;
extern int		done;

int			filenum, num_files;
char			*filenames[MAX_FILES];

/********************       ON_WINDOW1_DESTROY       ********************/
G_MODULE_EXPORT void
on_window1_destroy                      (GObject         *object,
                                        gpointer         user_data)
{
    done = 1;
}

/********************            ALLTRIM             ********************/
int
alltrim(char *string)
{
    int		i, first, last, ch;

    for (first = 0; (ch = string[first]) != '\0'; ++first)
    {
	if (!isspace(ch))
	    break;
    }
    i = 0;
    last = -1;
    while ((ch = string[first++]) != '\0')
    {
	string[i] = ch;
	if (!isspace(ch))
	    last = i;
	++i;
    }
    string[++last] = '\0';
    return(last);
}

/********************             ALERT              ********************/
void
alert(const char *fmt, ...)
{
    va_list		ap;
    char		temp[1024];
    GtkMessageDialog	*dialog;
    GtkLabel		*label;

    temp[sizeof(temp)-1] = '\0';
    va_start(ap, fmt);
    vsnprintf(temp, sizeof(temp)-1, fmt, ap);
    va_end(ap);
    dialog = (GtkMessageDialog *)gtk_builder_get_object(ui_xml, "alert_window");
    label = (GtkLabel *)gtk_builder_get_object(ui_xml, "alert_label");
    gtk_label_set_label(label, temp);
    gtk_widget_set_visible(GTK_WIDGET(dialog), 1);
}


/********************         ON_ALERT_CLOSE         ********************/
G_MODULE_EXPORT void
on_alert_close()
{
    GtkMessageDialog	*dialog;

    dialog = (GtkMessageDialog *)gtk_builder_get_object(ui_xml, "alert_window");
    gtk_widget_set_visible(GTK_WIDGET(dialog), 0);
}


/********************            ON_QUIT             ********************/
G_MODULE_EXPORT void
on_quit()
{
    done = 1;
}

/********************            ON_ABOUT            ********************/
G_MODULE_EXPORT void
on_about()
{
    alert("GTK-4 Image Viewer version %s", VERSION);
}

/********************                X               ********************/
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
    GError		*error;
    static GdkTexture	*texture = NULL;
    static GlyLoader	*loader = NULL;
    static GlyImage	*image = NULL;
    static GlyFrame	*frame = NULL;

    if (texture)
	g_object_unref((GObject *)texture);
    if (loader)
	g_object_unref((GObject *)loader);
    if (image)
	g_object_unref((GObject *)image);
    if (frame)
	g_object_unref((GObject *)frame);
    texture = NULL;
    loader = NULL;
    image = NULL;
    frame = NULL;

    error = NULL;
    gfile = g_file_new_for_path (filenames[num]);
    loader = gly_loader_new(gfile);
    image = gly_loader_load(loader, &error);
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

    texture = gly_gtk_frame_get_texture(frame);
    gtk_pic = (GtkPicture *)gtk_builder_get_object(ui_xml, "image_window");
    gtk_picture_set_paintable(gtk_pic, (GdkPaintable *)texture);
    
    window = (GtkWindow *)gtk_builder_get_object(ui_xml, "top_window");
    gtk_window_set_title(window, filenames[num]);

    g_object_unref((GObject *)gfile);
    filenum = num;
}

/********************            ON_PREV             ********************/
G_MODULE_EXPORT void
on_prev()
{
    if (filenum > 0)
	show_file(filenum-1);
    else
	alert("You are already at the first file");
}

/********************            ON_NEXT             ********************/
G_MODULE_EXPORT void
on_next()
{
    if (filenum >= num_files-1)
	alert("You are already at the last file");
    else
	show_file(filenum+1);
}

/********************            ON_FILL             ********************/
G_MODULE_EXPORT void
on_fill()
{
    GtkPicture	*pic;
    static int	can_shrink = TRUE;

    pic = (GtkPicture *)gtk_builder_get_object(ui_xml, "image_window");
    can_shrink = !can_shrink;
    gtk_picture_set_can_shrink(pic, can_shrink);
}
