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
#include "viewer.h"

extern GtkBuilder	*ui_xml;
extern int		done;
extern void		show_file(int filenum);
extern void		scale(int direction);

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
    int		widget_width, width, height, xoffset, yoffset;
    gdouble	scale_factor;
    char	*fname;

    if (scriptfp == NULL)
	scriptfp = fopen("magick.sh", "a");
    widget = (GtkWidget *)gtk_builder_get_object(ui_xml, "scroll_window");
    widget_width = gtk_widget_get_width(widget);
    scale_factor = (gdouble)image_width / (gdouble)widget_width;
    xoffset = (x * scale_factor) - 200;
    yoffset = (y * scale_factor) - 40;
    width = 400;
    height = 400;
    fname = filenames[filenum];
    fprintf(scriptfp, "magick FRAMES/%s -crop %dx%d+%d+%d, CROP/%s\n",
	fname, width, height, xoffset, yoffset, fname);
    fflush(scriptfp);
}
