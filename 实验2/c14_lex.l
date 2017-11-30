%{

enum yytype{ ID = 258, NUMBER, STRING, IF, ELSE, WHILE, RETURN, INT, STR, VOID,
			 PRINT, SCAN, CMP, ASSIGN};

enum cmp{ LES_CMP = 272, GRE_CMP, EQU_CMP, LESE_CMP, GREE_CMP, NEQU_CMP};

enum assign{ ADD_ASSIGN = 278, SUB_ASSIGN, MUL_ASSIGN, DIV_ASSIGN, REM_ASSIGN};

int yylval, lineno = 1;
char *sval;	//记录标识符、字符串
	
%}

ID 		[a-zA-Z_][a-zA-Z_0-9]*

INTEGER [0-9]+

STRING	\"([^\"]|(\\\"))*\"

COMMENT	"//"(.*)

SYMBOL 	[,%*+-=/{}\[\]\(\){}]

%%

if		{return IF;}
else	{return ELSE;}
while	{return WHILE;}
return	{return RETURN;}
int 	{return INT;}
str		{return STR;}
void	{return VOID;}
print	{return PRINT;}
scan	{return SCAN;}

"<"		{yylval = LES_CMP; return CMP;}
">"		{yylval = GRE_CMP; return CMP;}
"=="	{yylval = EQU_CMP; return CMP;}
"<="	{yylval = LESE_CMP; return CMP;}
">="	{yylval = GREE_CMP; return CMP;}
"!="	{yylval = NEQU_CMP; return CMP;}

"+="	{yylval = ADD_ASSIGN; return ASSIGN;}
"-="	{yylval = SUB_ASSIGN; return ASSIGN;}
"*="	{yylval = MUL_ASSIGN; return ASSIGN;}
"/="	{yylval = DIV_ASSIGN; return ASSIGN;}
"%="	{yylval = REM_ASSIGN; return ASSIGN;}

{ID}	{sval = strdup(yytext); return ID;}

{INTEGER}	{yylval = atoi(yytext); return NUMBER;}

{STRING}	{char *tem = strdup(yytext + 1); 
			 tem[yyleng - 2] = 0;
			 sval = strdup(tem);
			 free(tem);
			 return STRING;}

{SYMBOL}	{return yytext[0];}

{COMMENT}	{ /*ignore comment*/ }

[ \t]	{ /*ignore space*/ }

"\n"	++lineno;

. { fprintf(stderr, "Illegal character \'%s\' on line #%d\n", yytext, lineno); exit(1);}

%%

int yywrap(){
	return 1;
}
