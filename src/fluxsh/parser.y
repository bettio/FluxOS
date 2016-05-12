%{

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>

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

    int uid = getuid();
    struct passwd pw;
    struct passwd *result;
    char buf[512];
    getpwuid_r(uid, &pw, buf, 512, &result);

    fprintf(stderr, "[%s@%s %s] %c ", result->pw_name, hostname, cd, (uid ? '$' : '#'));
}

int cdCommandHandler(const char *command, char *const args[])
{
    if (chdir(args[1]) < 0) {
        fprintf(stderr, "FluxSH: cd: ");
        perror(args[1]);
    }
}

int executableCommandHandler(const char *command, char *const args[])
{
    const char *path[] = { "/bin", "/sbin", "/usr/bin", "/usr/sbin", NULL };

    int status;
    int pid = fork();
    if (!pid){
        char pathToExecutable[256];

        int i = 0;
        while (path[i] != NULL) {
            strcpy(pathToExecutable, path[i]);
            strcat(pathToExecutable, "/");
            strcat(pathToExecutable, command);
            if (access(pathToExecutable, X_OK) == 0) {
                break;
            }
            i++;
        }

        if ((path[i] == NULL) || execve(pathToExecutable, args, env) < 0) {
            fprintf(stderr, "FluxSH: cannot execute %s\n", command);
        }
        return EXIT_FAILURE;

    } else {
        waitpid(pid, &status, 0);
        return status;
    }
}


int commandHandler(const char *command, char *const args[])
{
    if (!strcmp(command, "cd")) {
        cdCommandHandler(command, args);
    } else {
        executableCommandHandler(command, args);
    }
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
       args[argsIndex - 1] = NULL;
       commandHandler($1, args + argsIndex);
       argsIndex = 254;
   }
   | CMDTOKEN
   {
       args[argsIndex] = $1;
       args[argsIndex - 1] = NULL;
       commandHandler($1, args + argsIndex);
       argsIndex = 254;
   }

%%

#include "lex.yy.c"

void yyerror(const char * message)
{
   fprintf(stderr, "%s\n", message);
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
   fprintf(stderr, " exit\n");

   return 0;
}

