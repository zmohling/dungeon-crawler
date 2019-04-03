%{
    #include <stdio.h>
    #include <string.h>

    #include "def_parser.tab.h"
    int line_num = 1;
%}

%%
[ \t]                         {;}
"BEGIN MONSTER"               {;}
"END"                         {;}
"NAME"                        { return TOKNAME; }
"DESC"                        { return TOKDESC; }
"COLOR"                       { return TOKCOLOR; }
"SPEED"                       { return TOKSPEED; }
"ABIL"                        { return TOKABIL; }
"HP"                          { return TOKHP; }
"DAM"                         { return TOKDAM; }
"SYMB"                        { return TOKSYMB; }
"RRTY"                        { return TOKRRTY; }
[0-9]+\+[0-9]d[0-9]+          { yylval.sval = strdup(yytext); return DICE; }
[0-9]+                        { yylval.ival = atoi(yytext); return NUMBER; }
([a-zA-Z](.)*)$                    { yylval.sval = strdup(yytext); return STRING; }
\n                            { line_num++; return ENDL; }
.                             {;}
%%

