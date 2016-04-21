%{

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


int argsIndex;
char **args;
char **env;

int    yylex(void);
void   yyerror(const char *);
//FILE * yyin;


void printPrompt()
{
    char hostname[16];
    gethostname(hostname, 16);

    char cd[64];
    getcwd(cd, 256);

    fprintf(stderr, "[@%s %s] %c ", hostname, cd, (getuid() ? '$' : '#'));
}

%}

%token CMDTOKEN
%token NEWLINE

%start source

%union {
     char *str;
     int ival;
}
%type<str> CMDTOKEN

%%

source
   : commandline
   | commandline source

commandline
   : commands NEWLINE
   {
       printPrompt();
   }
   | NEWLINE
   {
       printPrompt();
   }

args
   : CMDTOKEN args
   {
       args[argsIndex] = $1;
       argsIndex--;
   }
   | CMDTOKEN
   {
       args[argsIndex] = $1;
       argsIndex--;
   }

commands
   : command

command
   : CMDTOKEN args
   {
       args[argsIndex] = $1;

       int status;
       int pid = fork();
       if (!pid){
           char pathToExecutable[256];
           strcpy(pathToExecutable, "/bin/");
           strcat(pathToExecutable, $1);
           if (execve(pathToExecutable, args + argsIndex, env) < 0) {
               fprintf(stderr, "FluxSH: cannot execute %s\n", $1);
           }
       } else {
           waitpid(pid, &status, 0);
       }
       argsIndex = 254;
   }
   | CMDTOKEN
   {
       args[argsIndex] = $1;

       int status;
       int pid = fork();
       if (!pid){
           char pathToExecutable[256];
           strcpy(pathToExecutable, "/bin/");
           strcat(pathToExecutable, $1);
           if (execve(pathToExecutable, args + argsIndex, env) < 0) {
               fprintf(stderr, "FluxSH: cannot execute %s\n", $1);
           }
       } else {
           waitpid(pid, &status, 0);
       }
       argsIndex = 254;
   }

%%

#include "lex.yy.c"

void yyerror(const char * message)
{
   printf("%s\n", message);
}

int main(int argc, char *argv[], char *envp[])
{

   env = envp;

   args = (char **) malloc(sizeof(char *) * 256);
   memset(args, 0, sizeof(char *) * 256);
   argsIndex = 254;

   if(argc == 2) {
       yyin = fopen(argv[1], "rb");
   } else {
       yyin = (FILE *) 0;
   }

   printPrompt();
   yyparse();

   return 0;
}

