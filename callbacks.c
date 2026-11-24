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

extern viewer_app_t	*app;

/********************       ON_WINDOW1_DESTROY       ********************/
G_MODULE_EXPORT void
on_window1_destroy                      (GObject         *object,
                                        gpointer         user_data)
{
    app->done = true;
    g_application_quit(G_APPLICATION(app->app));
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

    temp[sizeof(temp)-1] = '\0';
    va_start(ap, fmt);
    vsnprintf(temp, sizeof(temp)-1, fmt, ap);
    va_end(ap);
    gtk_label_set_label(app->alert_label, temp);
    gtk_widget_set_visible(GTK_WIDGET(app->alert_dialog), true);
}


/********************         ON_ALERT_CLOSE         ********************/
G_MODULE_EXPORT void
on_alert_close()
{
    gtk_widget_set_visible(GTK_WIDGET(app->alert_dialog), false);
}


/********************            ON_QUIT             ********************/
G_MODULE_EXPORT void
on_quit()
{
    app->done = true;
    g_application_quit(G_APPLICATION(app->app));
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
    if (app->filenum > 0)
	show_file(app, app->filenum-1);
    else
	alert("You are already at the first file");
}

/********************            ON_NEXT             ********************/
G_MODULE_EXPORT void
on_next()
{
    if (app->filenum >= app->num_files-1)
	alert("You are already at the last file");
    else
	show_file(app, app->filenum+1);
}

/********************            ON_FILL             ********************/
G_MODULE_EXPORT void
on_fill()
{
    scale(app, SCALE_FILL, 0.0);
}
