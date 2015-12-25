%{

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


int argsIndex = 254;
char *args[256];
char **env;

int    yylex(void);
void   yyerror(const char *);
//FILE * yyin;


void printPrompt()
{
//    char hostname[16];
//    gethostname(hostname, 16);

    char cd[64];
    getcwd(cd, 256);

    fprintf(stderr, "[@%s %s] %c ", /*hostname*/"test", cd, (getuid() ? '$' : '#'));
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
           if (execve($1, args + argsIndex, env) < 0) {
               fprintf(stderr, "FluxSH: cannot execute %s\n", $1);
           }
       } else {
           waitpid(pid, &status, 0);
       }
   }
   | CMDTOKEN
   {
       printf("command with no args: %s\n", $1);
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
#if 0
   if(argc == 2) {
       yyin = fopen(argv[1], "rb");
   } else {
#endif
printf("Ciao\n");
       yyin = (FILE *) 0;
#if 0
   }
#endif
   printPrompt();
printf("ok\n");
   yyparse();
   return 0;
}

