#include <malloc.h>
#include <string.h>
#include <gtk/gtk.h>
#include "viewer.h"
#include "gtkbuilder.h"

GtkBuilder		*ui_xml;
GtkWindow		*top_window;

extern int		default_width, default_height;

extern void		init_help();
extern void		display_help();
extern void		on_prev();
extern void		on_next();
extern void		on_quit();
extern void		on_fill();
extern void		scale(int, float);
extern void		pinch_zoom(GtkGestureZoom *, double, GtkWidget *);
extern void		pinch_begin(GtkGestureZoom *, double, GtkWidget *);
extern void		pinch_end(GtkGestureZoom *, double, GtkWidget *);
extern void		file_dialog(), init_file_dialog();
extern GLogWriterOutput	my_logger(GLogLevelFlags, const GLogField *, gsize,
			    gpointer);

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

/*********************         KEY_PRESSED           ********************/
gboolean
key_pressed(GtkEventControllerKey *self, guint keyval, guint keycode,
    GdkModifierType state, gpointer user_data)
{
    gboolean	retcode;

    retcode = true;
    //fprintf(stderr, "key_pressed(): keyval=%04X  keycode=%04X  state=%X\n", keyval, keycode, state);
    switch(keyval | (state << 16))
    {
	case GDK_KEY_Right:
	case GDK_KEY_n:
	    on_next();
	    break;
	case GDK_KEY_Left:
	case GDK_KEY_p:
	    on_prev();
	    break;
	case GDK_KEY_minus:
	    scale(SCALE_DOWN, 0.0);
	    break;
	case GDK_KEY_plus:
	case GDK_KEY_plus | (GDK_SHIFT_MASK << 16):
	    scale(SCALE_UP, 0.0);
	    break;
	case GDK_KEY_1:
	    scale(SCALE_1_TO_1, 0.0);
	    break;
	case GDK_KEY_f:
	    scale(SCALE_FILL, 0.0);
	    break;
	case GDK_KEY_q:
	    on_quit();
	    break;
	case GDK_KEY_question:
	case GDK_KEY_question | (GDK_SHIFT_MASK << 16):
	    display_help();
	    break;
	case GDK_KEY_o | (GDK_CONTROL_MASK << 16):
	    // GDK_CONTROL_MASK
	    file_dialog();
	    break;
	default:
	    retcode = false;
    }

    return(retcode);
}

/********************              INIT              ********************/
void
init(char *ui_file, char *geometry)
{
    char		*s;
    GtkWidget		*picture;
    GtkGesture		*gesture;
    GtkEventController	*controller;

    if (!geometry)
	geometry = strdup(DEFAULT_GEOMETRY);
    s = strtok(geometry, "x");
    default_width = atoi(s);
    s = strtok(NULL, "x");
    default_height = atoi(s);

    gtk_init();
    if (ui_file)
	ui_xml = gtk_builder_new_from_file("Viewer-gtk4.ui");
    else
	load_ui();

    top_window = (GtkWindow *)gtk_builder_get_object(ui_xml, "top_window");
    gtk_widget_grab_focus((GtkWidget *)top_window);
    gtk_window_set_default_size(top_window, default_width, default_height);

    // Setup a key press event handler so we can capture stuff like arrow keys
    controller = gtk_event_controller_key_new();
    g_signal_connect(controller, "key-pressed", G_CALLBACK(key_pressed), top_window);
    gtk_event_controller_set_propagation_phase(controller, GTK_PHASE_BUBBLE);
    gtk_widget_add_controller(GTK_WIDGET(top_window), GTK_EVENT_CONTROLLER(controller));

    // Setup the handler for pinch / zoom events on the mouse pad.
    picture = GTK_WIDGET(gtk_builder_get_object(ui_xml, "image_window"));
    gesture = gtk_gesture_zoom_new();
    g_signal_connect(gesture, "scale-changed", G_CALLBACK(pinch_zoom), picture);
    g_signal_connect(gesture, "begin", G_CALLBACK(pinch_begin), picture);
    g_signal_connect(gesture, "end", G_CALLBACK(pinch_end), picture);
    gtk_event_controller_set_propagation_phase(GTK_EVENT_CONTROLLER(gesture),
	GTK_PHASE_BUBBLE);
    gtk_widget_add_controller(picture, GTK_EVENT_CONTROLLER(gesture));

    init_help();
    g_log_set_writer_func(my_logger, NULL, NULL);

    init_file_dialog();
    
    gtk_window_present(top_window);
}
