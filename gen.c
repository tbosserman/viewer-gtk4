#include <stdio.h>
#include <stdlib.h>

#define TRUE	1
#define FALSE	0

int
main(int argc, char *argv[])
{
    int		ch, prevch, column, bol;

    printf("static char *gtk_xml_buffer[] = {");
    column = 0;
    bol = TRUE;
    prevch = 0;
    while ((ch = getchar()) != EOF)
    {
	if (column >= 64 && prevch != '\\')
	{
	    fputs("\",", stdout);
	    column = 0;
	}
	if (column == 0)
	{
	    printf("\n    \"");
	    column = 4;
	}
	switch(ch)
	{
	    case '"':
		fputs("\\\"", stdout);
		column += 2;
		break;
	    case '\n':
		bol = TRUE;
		break;
	    case ' ':
	    case '\t':
		if (bol)
		    continue;
		putchar(ch);
		++column;
		break;
	    default:
		putchar(ch);
		++column;
		bol = FALSE;
	}
	prevch = ch;
    }
    printf("\",\n    NULL};\n");

    exit(0);
}
