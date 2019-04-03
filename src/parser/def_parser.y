%{
    #include <stdio.h>
    #include <stdlib.h>

    // Declare stuff from Flex that Bison needs to know about:
    extern int yylex();
    extern int yyparse();
    extern int line_num;
 
    void yyerror(const char *s);
%}

%union {
  int ival;
  float fval;
  char *sval;
}

%token BEGIN_MONSTER END ENDL
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
    : TOKNAME STRING ENDLS {
            printf("NAME %s\n", $2); free($2);
      }
    | END { exit(0); }
    ;
ENDLS:
  ENDLS ENDL
  | ENDL ;
%%

void yyerror(const char *str)
{
    fprintf(stderr,"error: %s at line %d\n", str, line_num);
}
 
int yywrap()
{
    return 1;
} 
  
int main()
{
    yyparse();
} 

