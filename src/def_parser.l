%{
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>

    //#include "def_compiler.tab.h"

    //extern int yylex();
    //extern int yylineno;
    //extern char* yytext;
%}

/* track line numbers*/
%option yylineno 

/* Character classes */
ALPHA [a-zA-Z]
SYMBOL [-_~|*/\[\]\.\{\}^+$/#"%&'!(),:;]
DIGIT [0-9]
LOGIC_VALUE [01zZxX]
ID_VALUE [^\n\r \t]
WS [ \t]
ENDL (\n|\n\r)

/* Lexer states */
%s long_string
%s string

%%
"RLG327 MONSTER DESCRIPTION"{WS}{DIGIT}           { ; }
"BEGIN MONSTER"                                   {  return BEGIN_MONSTER; }
"END"                                             {  return END; }
"NAME"                                            { 
                                                     BEGIN(string);
                                                     return TOKNAME; 
                                                  }
"DESC"                                            { 
                                                     BEGIN(long_string);
                                                     return TOKDESC; 
                                                  }
"COLOR"                                           { 
                                                     BEGIN(string);
                                                     return TOKCOLOR; 
                                                  }
"SPEED"                                           {  return TOKSPEED; }
"ABIL"                                            { 
                                                     BEGIN(string);
                                                     return TOKABIL; 
                                                  }
"HP"                                              {  return TOKHP; }
"DAM"                                             {  return TOKDAM; }
"SYMB"                                            { 
                                                     BEGIN(string);
                                                     return TOKSYMB; 
                                                  }
"RRTY"                                            {  return TOKRRTY; }



<long_string>(.{1,77}\n)+/(\.\n)                  { 
                                                     BEGIN(0);
                                                     yylval.sval = strdup(yytext); 
                                                     return STRING;
                                                  }
<string>({ALPHA})({ALPHA}|{DIGIT}|{SYMBOL}|{WS})* { 
                                                     BEGIN(0);
                                                     yylval.sval = strdup(yytext);
                                                     return STRING; 
                                                  }

{DIGIT}+"+"{DIGIT}+d{DIGIT}+                      {  
                                                     yylval.sval = strdup(yytext);
                                                     return DICE; 
                                                  }
{DIGIT}+$                                         {  
                                                     yylval.ival = atoi(yytext);
                                                     return DICE; 
                                                  }



[ \t]                                             { ; }
\n                                                { ; }
.                                                 { ; }
%%
