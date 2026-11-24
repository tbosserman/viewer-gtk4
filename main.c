#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <getopt.h>
#include "viewer.h"

#define APPLICATION	TRUE

char			*progname;
viewer_app_t		*app;

static char		*ui_file;

static struct option longopts[] =
{
    { "help",		no_argument,		NULL, 'h' },
    { "version",	no_argument,		NULL, 'V' },
    { "ui",		optional_argument,	NULL, 'u' },
    { "geometry",	required_argument,	NULL, 'g' },
    { NULL,		0,			NULL, 0   }
};

/********************             USAGE              ********************/
void
usage()
{
    fprintf(stderr, "usage: %s [-h|--help] [-V|--version] ", progname);
    fprintf(stderr, "[--ui[= ui_file]] [--geometry widthxheight ");
    fprintf(stderr, "image_file [...]\n");
    exit(1);
}

/********************          VERSION_INFO          ********************/
void
version_info()
{
    fprintf(stderr, "GTK4 Image Viewer version %s\n", VERSION);
    exit(1);
}

/********************          PROCESS_ARGS          ********************/
char *
process_args(viewer_app_t *app, int argc, char *argv[])
{
    int		i, ch;
    char	*geometry;

    geometry = NULL;
    while ((ch = getopt_long(argc, argv, "Vh?", longopts, NULL)) != -1)
    {
	switch(ch)
	{
	    case 'h':
		usage();
	    case 'V':
		version_info();
	    case 'u':
		if (optarg)
		    ui_file = optarg;
		else
		    ui_file = DEFAULT_UI_FILE;
		break;
	    case 'g':
		geometry = optarg;
		break;
	    default:
		usage();
	}
    }

    app->num_files = 0;
    for (i = optind; i < argc && i < MAX_FILES; ++i)
	app->filenames[app->num_files++] = g_strdup(argv[i]);
    return(geometry);
}

/********************              MAIN              ********************/
int
main(int argc, char *argv[])
{
    int		i;
    char	*geometry;

    progname = argv[0];
    for (i = strlen(progname) - 1; i >= 0 && progname[i] != '/'; --i);
    progname += (i + 1);

    if ((app = malloc(sizeof(viewer_app_t))) == NULL)
    {
	fprintf(stderr, "%s: Out of memory\n", progname);
	exit(1);
    }
    geometry = process_args(app, argc, argv);
    init(app, ui_file, geometry);
    app->done = false;
    show_file(app, 0);
#ifdef APPLICATION
    g_application_run(G_APPLICATION(app->app), 0, NULL);
#else
    while (!app->done)
	g_main_context_iteration (NULL, TRUE);
#endif

    exit(0);
}
