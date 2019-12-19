%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bracket.c"
#include "cheney.h"
//#include "KS_Graph.c"
#include "database.h"

extern int yylex();
void yyerror(char *msg);
float calc(float a, char c, float b);
void bracket(char *fun, char *str, char *valor);
void func_call(char* str);
%}


%union{
	int dval;
	char op;
	char str[150];
}

%token <str> NAME
%token <str> NUMBER
%token <op> OPERATOR
%token EOL AP FP ASSIGN SUM MIN TIMES DIV IF THEN ELSE GT LT

%type <str> expr
%type <str> condition
%type <str> factor
%type <str> term
%type <str> FUNC_CALL

%%

PROGRAM : STATEMENT {;}
	;

STATEMENT : STATEMENT STATEMENT
	| DECLARATION {;}
	| FUNC_CALL {
				// printf("%s", $1);
				func_call($1);
				}
	| EOL {;}
	;

DECLARATION : FUNC_ASSIGN_VAR {;}
	;

FUNC_CALL : NAME AP expr FP {
		char r[150] = "";
		char b[2] = "\0";
		char ap[2] = "(";
		char fp[2] = ")";
		//strcat(r, ap);
		strcat(r, $1);
		strcat(r, ap);
		strcat(r, $3);
		strcat(r, fp);
		//strcat(r, fp);
		strcpy($$, r);
		}
	| NAME AP FUNC_CALL FP {
		char r[150] = "";
		char b[2] = "\0";
		char ap[2] = "(";
		char fp[2] = ")";
		//strcat(r, ap);
		strcat(r, $1);
		strcat(r, ap);
		strcat(r, $3);
		strcat(r, fp);
		//strcat(r, fp);
		strcpy($$, r);
		}
	;

FUNC_ASSIGN_VAR : NAME AP NAME FP ASSIGN expr { 
		
		//printFunctions();
		bracket($1, $6, "\0");
	}
	;

expr : expr SUM expr {
		char r[150] = "";
		char a[2] = "+";
		char b[2] = "\0";
		char ap[2] = "(";
		char fp[2] = ")";
		strcat(r, a);
		strcat(r, ap);
		strcat(r, $1);
		strcat(r, fp);
		strcat(r, ap);
		strcat(r, $3);
		strcat(r, fp);
		strcpy($$, r);
		}
	| expr MIN expr {
		char r[150] = "";
		char a[2] = "-";
		char b[2] = "\0";
		char ap[2] = "(";
		char fp[2] = ")";
		strcat(r, a);
		strcat(r, ap);
		strcat(r, $1);
		strcat(r, fp);
		strcat(r, ap);
		strcat(r, $3);
		strcat(r, fp);
		strcpy($$, r);
		}
	| term
	| IF condition THEN expr ELSE expr {
		char r[150] = "";
		char ifa[3] = "if\0";
		char thena[5] = "then\0";
		char elsea[5] = "else\0";
		char b[2] = "\0";
		char ap[2] = "(";
		char fp[2] = ")";
		strcat(r, ifa);
		strcat(r, ap);
		strcat(r, $2);
		strcat(r, fp);
		strcat(r, thena);
		strcat(r, ap);
		strcat(r, $4);
		strcat(r, fp);
		strcat(r, elsea);
		strcat(r, ap);
		strcat(r, $6);
		strcat(r, fp);
		strcat(r, b);
		strcpy($$, r);
		}
	;

condition: expr ASSIGN expr {
		char r[150] = "";
		char a[2] = "=";
		char b[2] = "\0";
		char ap[2] = "(";
		char fp[2] = ")";
		strcat(r, ap);
		strcat(r, a);
		strcat(r, $1);
		strcat(r, $3);
		strcat(r, b);
		strcat(r, fp);
		strcpy($$, r);
		}
	| expr GT expr {
		char r[150] = "";
		char a[2] = ">";
		char b[2] = "\0";
		char ap[2] = "(";
		char fp[2] = ")";
		strcat(r, ap);
		strcat(r, a);
		strcat(r, $1);
		strcat(r, $3);
		strcat(r, b);
		strcat(r, fp);
		strcpy($$, r);
		}
	| expr LT expr {
		char r[150] = "";
		char a[2] = "<";
		char b[2] = "\0";
		char ap[2] = "(";
		char fp[2] = ")";
		strcat(r, ap);
		strcat(r, a);
		strcat(r, $1);
		strcat(r, $3);
		strcat(r, b);
		strcat(r, fp);
		strcpy($$, r);
		}
	;

term: term TIMES factor {
		char r[150] = "";
		char a[2] = "*";
		char b[2] = "\0";
		char ap[2] = "(";
		char fp[2] = ")";
		strcat(r, a);
		strcat(r, ap);
		strcat(r, $1);
		strcat(r, fp);
		strcat(r, ap);
		strcat(r, $3);
		strcat(r, fp);
		strcpy($$, r);
		}
	| term DIV factor {
		char r[150] = "";
		char a[2] = "/";
		char b[2] = "\0";
		char ap[2] = "(";
		char fp[2] = ")";
		strcat(r, a);
		strcat(r, ap);
		strcat(r, $1);
		strcat(r, fp);
		strcat(r, ap);
		strcat(r, $3);
		strcat(r, fp);
		strcpy($$, r);
		}
	| factor	{;}
	;

factor: AP expr FP 		 {/* $$ = $2 */;}
	| MIN factor         {/* $$ = -$2 */;}
	| NUMBER             {;}
	| NAME               {;}
	| FUNC_CALL          {/* $<str>$ = $1 */;}
	;

%%

void func_call(char* str){
	int result = functionCall(str);
	printf("%d\n", result);

}
float calc(float a, char c, float b){
	switch(c){
		case '+' :
			return a + b;
		case '-':
			return a - b;
	}
}

void bracket(char *fun, char *str, char *valor){
	char* r = converter_para_bracket(fun, str, valor);
	//printf("%s\n", str);
	storeFunction(fun, r);
}

void yyerror(char *msg) {
	fprintf(stderr, "%s\n", msg);
	exit(1);
}

int main(){
	yyparse();
	return 0;
}