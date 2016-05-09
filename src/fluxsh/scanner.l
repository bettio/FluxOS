%{
   char *strdupNoEscapes(const char *str)
   {
       int i = 0;
       int slen = strlen(str);
       char *newStr = malloc(slen + 1);
       char *outStr = newStr;
       while (*str) {
           if (*str != '\\') {
               *outStr = *str;
               outStr++;
           }
           str++;
       }
       return newStr;
   }
%}

%option noyywrap
%option pointer

CMDTOK ([a-zA-Z_.\/-]|\\[ ])*
SPACE [ \t]+
NL [\n]

%%

{SPACE} { }
{NL} { return NEWLINE; }
{CMDTOK}       { yylval.str = strdupNoEscapes(yytext); return CMDTOKEN; }

%%
