%{
    #include <stdio.h>
    #include <stdlib.h>

    // Declare stuff from Flex that Bison needs to know about:
    extern int yylex();
    extern int yyparse();
    extern int path_init(char **, char *);
    extern FILE *yyin;
 
    void yyerror(const char *s);
%}

%union {
  int ival;
  float fval;
  char *sval;
}

%token BEGIN_MONSTER END
%token TOKNAME
%token TOKDESC 
%token TOKCOLOR 
%token TOKSPEED 
%token TOKABIL 
%token TOKHP 
%token TOKDAM 
%token TOKSYMB 
%token TOKRRTY 
%token <sval> DICE 
%token <ival> NUMBER 
%token <sval> STRING 

%%
def_parser
    : TOKNAME STRING {
            printf("NAME %s\n", $2); free($2);
      }
    ;
%%
#include "util.h"

void yyerror(const char *str)
{
    fprintf(stderr,"error: %s", str);
}
 
int yywrap()
{
    return 1;
} 
  
int main()
{    
    char *s = "monster_desc.txt";
    char *path;
    path_init(&path, s);

	FILE *myfile = fopen("in.snazzle", "r");
	// make sure it's valid:
	if (!myfile) {
		printf("I can't open a.snazzle.file!\n");
		return -1;
	}
	// set lex to read from it instead of defaulting to STDIN:
	yyin = myfile;

	// parse through the input until there is no more:
	
	do {
		yyparse();
	} while (!feof(yyin));
} 

