%{
#include "y.tab.h"
#include <string.h>
%}

%option noinput nounput

%%
[0-9]+ {strcpy(yylval.str, yytext); return NUMBER;}
[\n] {return EOL;}
"(" {return AP;}
")" {return FP;}
"=" {return ASSIGN;}
"+" {return SUM;}
"-" {return MIN;}
"*" {return TIMES;}
"/" {return DIV;}
"<" {return LT;}
">" {return GT;}
"if" {return IF;}
"then" {return THEN;}
"else" {return ELSE;}
[a-zA-Z]+[a-zA-Z0-9]* {
                      int tam = strlen(yytext);
                      strcpy(yylval.str, yytext);
                      char a[2] = "\0";
                      strcat(yylval.str, a);
                      return NAME;}
%%

int yywrap(void) {return 1;}
