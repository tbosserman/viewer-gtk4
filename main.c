#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <getopt.h>
#include "viewer.h"

extern void		init(char *, char *);
extern void		show_file(int);
extern int		num_files;
extern char		*filenames[MAX_FILES];

char			*progname;
int			done;
char			*geometry;

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
void
process_args(int argc, char *argv[])
{
    int		i, ch;

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

    if (argc < 2)
	usage();
    num_files = 0;
    for (i = optind; i < argc && i < MAX_FILES; ++i)
	filenames[num_files++] = argv[i];
}

/********************              MAIN              ********************/
int
main(int argc, char *argv[])
{
    int		i;

    progname = argv[0];
    for (i = strlen(progname) - 1; i >= 0 && progname[i] != '/'; --i);
    progname += (i + 1);

    process_args(argc, argv);
    if (num_files == 0)
    {
	fprintf(stderr, "You didn't specify any files to display\n");
	exit(1);
    }
    init(ui_file, geometry);
    done = 0;
    show_file(0);
    while (!done)
	g_main_context_iteration (NULL, TRUE);

    exit(0);
}
