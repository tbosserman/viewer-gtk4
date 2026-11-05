#include <string.h>
#include <gtk/gtk.h>

#define NUM_PATTERNS	4

static char	*patterns[] = {
    "(slider) reported min height -2, but sizes must be >= 0",
    "(slider) reported min width -2, but sizes must be >= 0",
    "gdk_frame_timings_throttling_hint: runtime check failed",
    NULL
};

/*********************               X               ********************/

/*********************          MY_LOGGER            ********************/
GLogWriterOutput
my_logger(GLogLevelFlags level, const GLogField *fields, gsize nfields,
    gpointer user_data)
{
    int		i, priority;
    char	*pat;
    const gchar	*key, *msg;
    gconstpointer value;
    //gssize	len;

    // G_LOG_LEVEL_WARNING
    msg = NULL;
    priority = -1;
    for (i = 0; i < nfields; ++i)
    {
	key = fields[i].key;
	value = fields[i].value;
	if (strcmp(key, "MESSAGE") == 0)
	    msg = value;

	if (strcmp(key, "PRIORITY") == 0)
	    priority = atoi(value);
	else if (strcmp(key, "MESSAGE") == 0)
	    msg = value;

	//fprintf(stderr, "key='%s'  value='%s'\n", key, (char *)value);
    }

    // See if it's one of the messages we're trying to suppress
    pat = NULL;
    if (level == G_LOG_LEVEL_WARNING && priority == 4)
    {
	for (i = 0; (pat = patterns[i]) != NULL; ++i)
	{
	    if (strstr(msg, pat) != NULL)
		break;
	}
    }

    if (pat)
	return G_LOG_WRITER_HANDLED;

    //return G_LOG_WRITER_UNHANDLED;
    return g_log_writer_default(level, fields, nfields, user_data);
}

