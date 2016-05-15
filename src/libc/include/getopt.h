#ifndef _GETOPT_H_
#define _GETOPT_H_

extern int opterr;
extern int optind;
extern int optopt;
extern char *optarg;

int getopt(int argc, char * const argv[], const char *optstring);

#endif
