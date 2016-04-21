%{

%}

%option noyywrap
%option pointer

CMDTOK [a-zA-Z_.\/-]*
SPACE [ \t]+
NL [\n]

%%

{SPACE} { }
{NL} { return NEWLINE; }
{CMDTOK}       { yylval.str = strdup(yytext); return CMDTOKEN; }

%%
