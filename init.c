#include <malloc.h>
#include <string.h>
#include <gtk/gtk.h>
#include "viewer.h"
#include "gtkbuilder.h"

#define PREV	0
#define NEXT	1
#define FILL	2
#define QUIT	3

GtkBuilder		*ui_xml;
extern void		on_prev();
extern void		on_next();
extern void		on_quit();
extern void		on_fill();

typedef struct {
    char	*fmt_string;
    int		action;
} variant_t;

variant_t variants[] = {
    { "P|p",		PREV },
    { "Left",		PREV },
    { "Up",		PREV },
    { "N|n",		NEXT },
    { "Right",		NEXT },
    { "Down",		NEXT },
    { "F|f",		FILL },
    { "Q|a",		QUIT },
    { NULL,			  0    }
};

/********************            LOAD_UI             ********************/
void
load_ui()
{
    int		i, len, total_len;
    char	*buf, *from, *to;

    total_len = 0;
    for (i = 0; gtk_xml_buffer[i] != NULL; ++i)
	total_len += strlen(gtk_xml_buffer[i]);

    buf = to = malloc(total_len+1);
    for (i = 0; (from = gtk_xml_buffer[i]) != NULL; ++i)
    {
	len = strlen(from);
	memcpy(to, from, len);
	to += len;
    }
    *to = '\0';
    ui_xml = gtk_builder_new_from_string(buf, total_len);
    free(buf);
}

/********************           KEY_ACTION           ********************/
gboolean
key_action(GtkWidget *w, GVariant *v, gpointer p)
{
    variant_t	*vp;

    vp = (variant_t *)p;
    switch(vp->action)
    {
	case PREV: on_prev(); break;
	case NEXT: on_next(); break;
	case FILL: on_fill(); break;
	case QUIT: on_quit(); break;
    }
    return(1);
}

/********************              INIT              ********************/
void
init(char *ui_file, char *geometry)
{
    int			i, width, height;
    char		*s;
    GtkWindow		*window;
    GtkShortcut		*shortcut;
    GtkShortcutTrigger  *trigger;
    GtkShortcutAction   *action;
    GtkWidgetClass      *class;

    if (!geometry)
	geometry = strdup(DEFAULT_GEOMETRY);
    s = strtok(geometry, "x");
    width = atoi(s);
    s = strtok(NULL, "x");
    height = atoi(s);

    gtk_init();
    if (ui_file)
	ui_xml = gtk_builder_new_from_file("Viewer-gtk4.ui");
    else
	load_ui();

    window = (GtkWindow *)gtk_builder_get_object(ui_xml, "top_window");
    gtk_window_set_default_size(window, width, height);
    gtk_widget_grab_focus((GtkWidget *)window);
    class = GTK_WIDGET_GET_CLASS(GTK_WIDGET(window));
    for (i = 0; variants[i].fmt_string != NULL; ++i)
    {
	trigger = gtk_shortcut_trigger_parse_string(variants[i].fmt_string);
	action = gtk_callback_action_new(key_action, &variants[i], NULL);
	shortcut = gtk_shortcut_new(trigger, action);
	gtk_widget_class_add_shortcut(class, shortcut);
    }
    
    gtk_window_present(window);
}
