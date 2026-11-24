#ifndef TRUE
# define FALSE		0
# define TRUE		1
#endif
#define MAX_FILES	2000
#define VERSION		"3.0"
#define DEFAULT_UI_FILE	"Viewer-gtk4.ui"
#define DEFAULT_GEOMETRY "1024x768"
#define PREV		0
#define NEXT		1
#define FILL		2
#define QUIT		3
#define SCALE_UP	4
#define SCALE_DOWN	5
#define SCALE_FILL	6
#define SCALE_1_TO_1	7
#define SCALE_SET	8

typedef struct {
    int			activated;
    int			filenum;
    int			num_files;
    int			done;
    int			default_width;
    int			default_height;
    char		*filenames[MAX_FILES];
    char		*progname;
    GtkApplication	*app;
    GtkWindow		*top_window;
    GtkWindow		*help_window;
    GtkScrolledWindow	*scroll_window;
    GtkPicture		*image_window;
    GtkFileDialog	*file_dialog;
    GtkMessageDialog	*alert_dialog;
    GtkLabel		*alert_label;
    GtkBuilder		*ui;
} viewer_app_t;

extern void		init(viewer_app_t *, char *, char *);
extern void		file_dialog(viewer_app_t *);
extern GtkFileDialog	*init_file_dialog();
extern void		show_file(viewer_app_t *, int);
extern void		scale(viewer_app_t *, int, float);
extern void		init_help(viewer_app_t *);
extern void		display_help(viewer_app_t *);
extern void		on_prev();
extern void		on_next();
extern void		on_quit();
extern void		on_fill();
extern void		pinch_zoom(GtkGestureZoom *, double, GtkWidget *);
extern void		pinch_begin(GtkGestureZoom *, double, GtkWidget *);
extern void		pinch_end(GtkGestureZoom *, double, GtkWidget *);
extern void		remove_files(viewer_app_t *, int);
extern void		alert(const char *fmt, ...);

extern GLogWriterOutput my_logger(GLogLevelFlags, const GLogField *, gsize,
			    gpointer);
