%{
    #include <stdio.h>
    #include <string.h>

    #include <sys/stat.h>
    #include <sys/types.h>

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
"RLG327 MONSTER DESCRIPTION"{WS}{DIGIT}           {  ; }
"BEGIN MONSTER"                                   {  printf("\n"); }
"END"                                             {  ; }
"NAME"                                            { 
                                                     BEGIN(string);
                                                     ; 
                                                  }
"DESC"                                            { 
                                                     BEGIN(long_string);
                                                     ; 
                                                  }
"COLOR"                                           { 
                                                     BEGIN(string);
                                                     ; 
                                                  }
"SPEED"                                           {  ; }
"ABIL"                                            { 
                                                     BEGIN(string);
                                                     ; 
                                                  }
"HP"                                              {  ; }
"DAM"                                             {  ; }
"SYMB"                                            { 
                                                     BEGIN(string);
                                                     ; 
                                                  }
"RRTY"                                            {  ; }



<long_string>(.{1,77}\n)+/(\.\n)                  { 
                                                     BEGIN(0);
                                                     printf("%s", yytext);
                                                  }
<string>({ALPHA})({ALPHA}|{DIGIT}|{SYMBOL}|{WS})* { 
                                                     BEGIN(0);
                                                     printf("%s\n", yytext);
                                                  }

{DIGIT}+"+"{DIGIT}+d{DIGIT}+                      {  
                                                     printf("%s\n", yytext);
                                                  }
{DIGIT}+$                                         {  
                                                     printf("%d\n", atoi(yytext));
                                                  }



[ \t]                                             { ; }
\n                                                { ; }
.                                                 { ; }
%%

int main(int ac, char **av)
{
    char *path, *filename = "monster_desc.txt";
    char *home = getenv("HOME");
    char *dir = (char *) malloc(strlen(home) + strlen("/.rlg327/") + 1);
    strcpy(dir, home);
    strcat(dir, "/.rlg327/");

    if (mkdir(dir, 0755)) {
        /* errno == 17 is an existing directory */
        if (errno != 17) {
            fprintf(stderr,
                    "Error: ~/.rlg327/ could not be found nor created. (%d)\n",
                    errno);
            exit(-1);
        }
    }

    path = (char *) malloc(strlen(home) + strlen("/.rlg327/") + strlen(filename) + 1);
    strcpy(path, home);
    strcat(path, "/.rlg327/");
    strcat(path, filename);

    free(dir);

    FILE *f;
    if ((f = fopen(path, "r"))) {
        yyset_in(f);
        yylex();
        fclose(f);
    } else {
        perror("Error: ");
        return (-1);
    }

    free(path);

    return (0);
}
