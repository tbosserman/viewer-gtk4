#include <malloc.h>
#include <string.h>
#include <gtk/gtk.h>
#include "viewer.h"
#include "gtkbuilder.h"

/********************            LOAD_UI             ********************/
GtkBuilder *
load_ui()
{
    GtkBuilder	*ui_xml;
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
    return(ui_xml);
}

/*********************         KEY_PRESSED           ********************/
gboolean
key_pressed(GtkEventControllerKey *self, guint keyval, guint keycode,
    GdkModifierType state, gpointer user_data)
{
    extern viewer_app_t	*app;
    gboolean		retcode;

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
	    scale(app, SCALE_DOWN, 0.0);
	    break;
	case GDK_KEY_plus:
	case GDK_KEY_plus | (GDK_SHIFT_MASK << 16):
	    scale(app, SCALE_UP, 0.0);
	    break;
	case GDK_KEY_1:
	    scale(app, SCALE_1_TO_1, 0.0);
	    break;
	case GDK_KEY_f:
	    scale(app, SCALE_FILL, 0.0);
	    break;
	case GDK_KEY_q:
	    on_quit();
	    break;
	case GDK_KEY_question:
	case GDK_KEY_question | (GDK_SHIFT_MASK << 16):
	    display_help(app);
	    break;
	case GDK_KEY_o | (GDK_CONTROL_MASK << 16):
	    // GDK_CONTROL_MASK
	    file_dialog(app);
	    break;
	default:
	    retcode = false;
    }

    return(retcode);
}

/********************            ACTIVATE            ********************/
static void
activate(GApplication *gapp)
{
    extern viewer_app_t	*app;
    GtkApplication	*gtk_app;

    if (app->activated)
	return;

    gtk_app = GTK_APPLICATION(gapp);
    gtk_application_add_window(gtk_app, app->top_window);
    gtk_application_add_window(gtk_app, GTK_WINDOW(app->alert_dialog));
    gtk_application_add_window(gtk_app, app->help_window);
    gtk_window_present(app->top_window);
    app->activated = true;
}

/********************              INIT              ********************/
void
init(viewer_app_t *app, char *ui_file, char *geometry)
{
    char		*s;
    GtkWidget		*picture;
    GtkGesture		*gesture;
    GtkEventController	*controller;
    GError		*error;

    app->activated = false;
    if (!geometry)
	geometry = strdup(DEFAULT_GEOMETRY);
    s = strtok(geometry, "x");
    app->default_width = atoi(s);
    s = strtok(NULL, "x");
    app->default_height = atoi(s);

    gtk_init();
    if (ui_file)
	app->ui = gtk_builder_new_from_file("Viewer-gtk4.ui");
    else
	app->ui = load_ui();

    app->top_window = GTK_WINDOW(gtk_builder_get_object(app->ui, "top_window"));
    app->alert_dialog = GTK_MESSAGE_DIALOG(gtk_builder_get_object(app->ui,
	"alert_window"));
    app->alert_label = GTK_LABEL(gtk_builder_get_object(app->ui, "alert_label"));
    app->scroll_window = GTK_SCROLLED_WINDOW(gtk_builder_get_object(app->ui, 
	"scroll_window"));
    app->file_dialog = init_file_dialog();

    gtk_widget_grab_focus(GTK_WIDGET(app->top_window));
    gtk_window_set_default_size(app->top_window, app->default_width,
	app->default_height);

    // Setup a key press event handler so we can capture stuff like arrow keys
    controller = gtk_event_controller_key_new();
    g_signal_connect(controller, "key-pressed", G_CALLBACK(key_pressed),
	app->top_window);
    gtk_event_controller_set_propagation_phase(controller, GTK_PHASE_BUBBLE);
    gtk_widget_add_controller(GTK_WIDGET(app->top_window),
	GTK_EVENT_CONTROLLER(controller));

    // Setup the handler for pinch / zoom events on the mouse pad.
    picture = GTK_WIDGET(gtk_builder_get_object(app->ui, "image_window"));
    app->image_window = GTK_PICTURE(picture);
    gesture = gtk_gesture_zoom_new();
    g_signal_connect(gesture, "scale-changed", G_CALLBACK(pinch_zoom), picture);
    g_signal_connect(gesture, "begin", G_CALLBACK(pinch_begin), picture);
    g_signal_connect(gesture, "end", G_CALLBACK(pinch_end), picture);
    gtk_event_controller_set_propagation_phase(GTK_EVENT_CONTROLLER(gesture),
	GTK_PHASE_BUBBLE);
    gtk_widget_add_controller(picture, GTK_EVENT_CONTROLLER(gesture));

    init_help(app);
    g_log_set_writer_func(my_logger, NULL, NULL);

    app->app = gtk_application_new("com.bossermate.viewer", 
	G_APPLICATION_DEFAULT_FLAGS);
    g_object_connect(G_OBJECT(app->app), "signal::activate",
	activate, NULL);
    error = NULL;
    g_application_register(G_APPLICATION(app->app), NULL, &error);
    if (error)
    {
	fprintf(stderr, "registration failed: %s\n", error->message);
	exit(1);
    }
    
    // This should maybe go in an "activate" callback?
    // gtk_window_present(app->top_window);
}
