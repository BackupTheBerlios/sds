#include <SharedData.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <stdio.h>

static struct option const long_options[] = {
	{"help",      no_argument,       0, 'h'},
	{"version",   no_argument,       0, 'V'},
	{"warranty",  no_argument,       0, 'W'},
	{"copyright", no_argument,       0, 'C'},
	{"verbose",   no_argument,       0, 'v'},
	{"on",        no_argument,       0, 'i'},
	{"off",       no_argument,       0, 'o'},
	{"toggle",    no_argument,       0, 't'},
	{0, 0, 0, 0}
};

char verstring[] = "0.04";

extern char *program_invocation_short_name;

void warranty(void)
{
	printf("This program is free software; you can redistribute it"
               " and/or modify it\n"
               "under the terms of the GNU General Public License as"
	       " published by the \n"
	       "Free Software Foundation; either version 2 of the License,"
	       " or (at your \n"
	       "option) any later version.\n"
	       "\n"
	       "This program is distributed in the hope that it will be"
	       " useful, but \n"
	       "WITHOUT ANY WARRANTY; without even the implied warranty of"
	       " MERCHANTABILITY\n"
	       "or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General"
	       " Public License \n"
	       "for more details.\n"
	       "\n"
	       "You should have received a copy of the GNU General Public"
	       " License along \n"
	       "with this program; if not, write to the Free Software"
	       " Foundation, Inc., \n"
	       "59 Temple Place Suite 330, Boston, MA 02111-1307 USA.\n");
	exit(0);
}

void usage(void)
{
	fprintf(stderr,
		"usage: %s [-v] [-i|-o|-t]\n"
		"Options:\n"
		"\t-i,    --on         enable log mechanism\n"
		"\t-o,    --off        disable log mechanism\n"
		"\t-t,    --toggle     on <-> off\n"
		"\t-v,    --verbose    add comments\n"
		"\t-h,-?, --help       this message\n"
		"\t-V,    --version    display version information and exit.\n"
		"\t-W,    --warranty   display licensing terms.\n"
		"\t-C,    --copyright\n\n"
		"Use this program to change the log-state of the sdf-file.\n"
		"Run without -i, -o or -t to display current state.\n"
                "Maybe you inadvertentely switched logging off when a program"
                " crashed \n"
		"or was interrupted.\n"
                "Use with caution! This program can change history!\n",
		program_invocation_short_name);
	exit(0);
}

void copyright(void)
{
	printf("%s version %s, Copyright (C) 2000 Roland Schaefer"
	       "%s comes with ABSOLUTELY NO WARRANTY;\n"
	       "This is free software, and you are welcome to"
	       " redistribute it\n"
	       "under certain conditions; type `%s -w' for details.\n",
	       program_invocation_short_name,verstring,
	       program_invocation_short_name,program_invocation_short_name);
	exit(0);
}

void version(void)
{
	fprintf(stderr, "%s version %s\n", program_invocation_short_name,
		verstring);
	
	exit(0);
}


int main(int argc,char **argv)
{
	int c, ols, opt, verb;

	opt=TELLLOG;   // logstates #defined in SharedData.h
	verb=0;

	while ((c = getopt_long(argc, argv, "iotvh?VWC",
				long_options, (int *) 0)) != EOF) {
		switch (c) {
		case 0:
			break;
		case 'W':
			warranty();
			break;
		case 'V':
			version();
			break;
		case 'C':
			copyright();
			break;
		case 'i':
			opt=LOGON;
			break;
		case 'o':
			opt=LOGOFF;
			break;
		case 't':
		        opt=TOGGLELOG;
			break;
		case 'v':
			verb=1;
			break;
		case 'h':
		case '?':
		default:
			usage();
			break;
		}
	}

	// off we go..
	SharedData sd;

	switch (opt) {
	case LOGON:
		if (verb)
			printf("Trying to set log=ON..\n");
		ols=sd.logstate(opt);
		break;
	case LOGOFF:
		if (verb)
			printf("Trying to set log=OFF..\n");
		ols=sd.logstate(opt);
		break;
	case TOGGLELOG:
		if (verb)
			printf("Trying to toggle log state..\n");
		ols=sd.logstate(opt);
		break;
	case TELLLOG:
		if (verb)
			printf("Analyzing log state..\n");
		ols=sd.logstate(opt);
		printf("Log is currently set %s\n",(ols ? "ON" : "OFF"));
		break;
	default:
		break;
	}

	if ((opt!=TELLLOG) && verb)
		printf("Log was previously set %s\n",(ols ? "ON" : "OFF"));

}














