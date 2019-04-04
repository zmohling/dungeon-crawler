%{
    #include <stdio.h>
    #include <string.h>

    extern int yylex();
    extern int yylineno;
    extern char* yytext;
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
"BEGIN MONSTER"                                   { ; }
"END"                                             { ; }
"NAME"                                            { 
                                                     BEGIN(string);
                                                     printf("NAME:\n"); 
                                                  }
"DESC"                                            { 
                                                     BEGIN(long_string);
                                                     printf("DESC:\n"); 
                                                  }
"COLOR"                                           { 
                                                     BEGIN(string);
                                                     printf("COLOR:\n"); 
                                                  }
"SPEED"                                           {  printf("SPEED:\n"); }
"ABIL"                                            { 
                                                     BEGIN(string);
                                                     printf("ABIL:\n"); 
                                                  }
"HP"                                              {  printf("HP:\n"); }
"DAM"                                             {  printf("DAM:\n"); }
"SYMB"                                            { 
                                                     BEGIN(string);
                                                     printf("SYMB:\n"); 
                                                  }
"RRTY"                                            {  printf("RRTY:\n"); }



<long_string>(.{1,77}\n)+/(\.\n)                  { 
                                                     BEGIN(0);
                                                     printf("\t --- %s --- \n", yytext); 
                                                  }
<string>({ALPHA})({ALPHA}|{DIGIT}|{SYMBOL}|{WS})* { 
                                                     BEGIN(0);
                                                     printf("\t%s\n", yytext); 
                                                  }

{DIGIT}+"+"{DIGIT}+d{DIGIT}+                      {  printf("\t%s\n", yytext); }
{DIGIT}+$                                         {  printf("\t%d\n", atoi(yytext)); }



[ \t]                                             { ; }
\n                                                { ; }
.                                                 { ; }
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

