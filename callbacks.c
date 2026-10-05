#include <stdio.h>
#include <math.h>
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
#include "viewer.h"

extern GtkBuilder	*ui_xml;
extern int		done;
extern void		show_file(int filenum);
extern void		scale(int direction);
extern int		get_cursor_width();
extern int		get_cursor_height();

extern int		filenum, num_files, image_width, image_height;
extern char		*filenames[MAX_FILES];

static FILE		*scriptfp = NULL;

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
    scale(SCALE_FILL);
}

G_MODULE_EXPORT void
click_event(GtkEventController *gesture, gdouble x, gdouble y, gpointer data)
{
    GtkWidget	*widget;
    int		widget_width, widget_height, width, height, xoffset, yoffset;
    int		cursor_width, cursor_height, extra_x, extra_y;
    gdouble	scale_factor, image_aspect, window_aspect;
    char	*fname;

    if (scriptfp == NULL)
	scriptfp = fopen("magick.sh", "a");

    // The image being displayed may not have the same aspect ratio as the
    // window displaying it. So we need to figure out if there is a "band" at
    // either the top / bottom, or left / right. If there is such a band, the
    // size of it must be subtracted from the XY coordinates when calculating
    // xoffset and yoffset.
    // NOTE: right now (2026-10-04) this only handles images which are
    // wider than they are tall. Gotta fix that!
    widget = (GtkWidget *)gtk_builder_get_object(ui_xml, "scroll_window");
    widget_width = gtk_widget_get_width(widget);
    widget_height = gtk_widget_get_height(widget);
    image_aspect = (gdouble)image_width / (gdouble)image_height;
    window_aspect = (gdouble)widget_width / (gdouble)widget_height;
    scale_factor = (gdouble)image_width / (gdouble)widget_width;
    extra_x = extra_y = 0;
    if (fabs(image_aspect - window_aspect) > 0.01)
    {
	if (image_aspect > window_aspect) // empty band is at top
	{
	    double band_height;
	    band_height = (double)image_height / scale_factor;
	    extra_y = ((double)widget_height - band_height) / 2.0;
	}
	else // empty band is at left
	{
	    double band_width = (double)widget_height / image_aspect;
	    extra_x = ((double)widget_width - band_width) / 2.0;
	}
    }

    cursor_width = get_cursor_width() * scale_factor;
    cursor_height = get_cursor_height() * scale_factor;
    //cursor_width = get_cursor_width();
    //cursor_height = get_cursor_height();
    xoffset = ((x - extra_x) * scale_factor) - (cursor_width / 2) - 20;
    yoffset = ((y - extra_y) * scale_factor) - (cursor_height / 2) - 20;
    width = cursor_width + 40;
    height = cursor_height + 40;
    printf("\nclick_event: scale factor=%.2f\n", scale_factor);
    printf("hot_X=%.2f  hot_Y=%.2f  cursor_width=%d  cursor_height=%d\n",
	x, y, cursor_width, cursor_height);
    printf("extra_x=%d  extra_y=%d\n", extra_x, extra_y);
    printf("xoffset=%d  yoffset=%d  width=%d  height=%d\n\n", xoffset, yoffset,
	width, height);
    fname = filenames[filenum];
    fprintf(scriptfp, "magick FRAMES/%s -crop %dx%d+%d+%d CROP/%s\n",
	fname, width, height, xoffset, yoffset, fname);
    fflush(scriptfp);
}
