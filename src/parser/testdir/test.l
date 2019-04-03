%{
    #include <stdio.h>
    #include <string.h>

    extern int yylex();
    extern int yylineno;
    extern char* yytext;
%}

%%
[ \t]                         {;}
"BEGIN MONSTER"               {;}
"END"                         {;}
"NAME"                        { printf("NAME:\n"); }
"DESC"                        { printf("DESC:\n"); }
"COLOR"                       { printf("COLOR:\n"); }
"SPEED"                       { printf("SPEED:\n"); }
"ABIL"                        { printf("ABIL:\n"); }
"HP"                          { printf("HP:\n"); }
"DAM"                         { printf("DAM:\n"); }
"SYMB"                        { printf("SYMB:\n"); }
"RRTY"                        { printf("RRTY:\n"); }
[0-9]+\+[0-9]d[0-9]+          { printf("\t%s\n", yytext); }
[0-9]+                        { printf("\t%d\n", atoi(yytext)); }
^[a-zA-Z][^\n]*/(\.)                       { printf("\t%s\n", yytext); }
\n                            {;}
.                             {;}
%%

int yywrap(void)
{
	return 1;			
}

int main (void) {
    int s = yylex();
    while(s) {
        yylex();
    }
}

