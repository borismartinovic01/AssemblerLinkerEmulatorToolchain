%{
    #include <vector>
    #include "../inc/structsDefs.hpp"
    #include "machine.tab.hpp"

    using namespace std;
    int counter = 1;
%}
%option noyywrap
%option nounput
%%
[ \t]                               ;
"+"                                 { return PLUS; }
"$"                                 { return DOLLAR; }
"["                                 { return LBRACKET; }
"]"                                 { return RBRACKET; }
","                                 { return COMMA; }
#.*                                 { return COMMENT; }
halt                                { return I_HALT; }
int                                 { return I_INT; }
iret                                { return I_IRET; }
call                                { return I_CALL; }
ret                                 { return I_RET; }
jmp                                 { return I_JMP; }
beq                                 { return I_BEQ; }
bne                                 { return I_BNE; }
bgt                                 { return I_BGT; }
push                                { return I_PUSH; }
pop                                 { return I_POP; }
xchg                                { return I_XCHG; }
add                                 { return I_ADD; }
sub                                 { return I_SUB; }
mul                                 { return I_MUL; }
div                                 { return I_DIV; }
not                                 { return I_NOT; }
and                                 { return I_AND; }
or                                  { return I_OR; }
xor                                 { return I_XOR; }
shl                                 { return I_SHL; }
shr                                 { return I_SHR; }
ld                                  { return I_LD; }
st                                  { return I_ST; }
csrrd                               { return I_CSRRD; }
csrwr                               { return I_CSRWR; }
".global"                           { return D_GLOBAL; }
".extern"                           { return D_EXTERN; }
".section"                          { return D_SECTION; }
".word"                             { return D_WORD; }
".skip"                             { return D_SKIP; }
".end"                              { return D_END; }
[a-zA-Z_][a-zA-Z0-9_]*[ \t]*:       {
    yylval.label = strdup(yytext);

    if (yylval.label != NULL) {
        int i;
        for(i = 0; yylval.label[i] != '\0'; i++){
            char c = yylval.label[i];

            if(c != ' ' && c != ':' && c != '\t') continue;

            yylval.label[i] = '\0';
            return LABEL;;
        }
    }

    return LABEL;
}
%r([0-9]|1[0-5])                     { yylval.reg = atoi(yytext + 2); return REG; }
%sp                                  { yylval.reg = 14; return REG; }
%pc                                  { yylval.reg = 15; return REG; }
%status                              { yylval.reg = 0; return REG; }
%handler                             { yylval.reg = 1; return REG; }
%cause                               { yylval.reg = 2; return REG; }
0                                   { yylval.literal = 0; return LITERAL; }
[1-9][0-9]*                         { yylval.literal = atoi(yytext); return LITERAL; }
0[0-9]+                             { yylval.literal = strtol(yytext + 1, NULL, 8); return LITERAL; }
0x[0-9a-fA-F]+                      { yylval.literal = strtol(yytext + 2, NULL, 16); return LITERAL; }
\"[^"]+\"                           { yytext[strlen(yytext)-1] = '\0'; yylval.swrd = strdup(yytext + 1); return STRING; }
[a-zA-Z_.][a-zA-Z0-9_.]*            { yylval.swrd = strdup(yytext); if (yylval.swrd == NULL) /* return YYerror; */ return SWRD; return SWRD; }
\n                                  { ++counter; BEGIN(INITIAL); return ENDL; }
.                                   ;
%%