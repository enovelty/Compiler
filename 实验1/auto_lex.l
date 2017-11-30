/*	李昂 2016220304005 
*	关键字编号从300开始，标识符（ID）编号258，字符串261 
*	注释不输出 
*
*/
%{
int nextToken;

enum {ID = 258, INT, FLO, STR, SYM, OPE, EQU, GEQU, LEQU, AND, OR, INVERT, ERROR};	 
		
char *keywords[] = {"abstract",    "case",        "catch",       "class",       "def",
		    "do",          "else",        "extends",     "false",       "final",
		    "finally",     "for",         "forSome",     "if",          "implicit",
		    "import",      "lazy",        "macro",       "match",       "new",
		    "null",        "object",      "override",    "package",     "private",
		    "protected",   "return",      "sealed",      "super",       "this",
		    "throw",       "trait",       "try",         "true",        "type",
		    "val",         "var",         "while",       "with",        "yield",
		    "_",  ":",  "=",  "=>",  "<-",  "<:",  "<%",   ">:",  "#",  "@",
		    0};
		    
void checkKeywords (char *pword)
{
	int i = 0;
	while (keywords[i] != 0)
	{
		char *pkeyword = keywords[i];
		if (strcmp(pword, pkeyword) == 0)
		{
			nextToken = 300 + i;
			return ;
		}
		i++;
	}
}
		    
%}

INTEGER 	[+-]?[0-9]?[0-9]*

FLOAT 		[+-]?[0-9]+"."[0-9]*

SYMBOL 		[~!@#$%^&|*+-<>:?=/{}\[\]\(\)]

STRING		\"([^\"]|(\\\"))*\"

OPERATER	"=="|">="|"<="|"&&"|"!"|"!="
	
B_COMMENT 	"/*"([^\*]|(\*)+[^/])*"*/"

L_COMMENT	"//"(.*)

KEYWORDS1	"abstract"|"case"|"catch"|"class"|"def"|"do"|"else"|"extends"|"false"|"final"|"finally"|"for"|"forSome"|"if"|"implicit"|"import"     
KEYWORDS2	"lazy"|"macro"|"match"|"new"|"null"|"object"|"override"|"package"|"private"|"protected"|"return"|"sealed"|"super"|"this"|"throw"|"trait"|"try"    
KEYWORDS3	"true"|"type"|"val"|"var"|"while"|"with"|"yield"|"_"|":"|"="|"=>"|"<-"|"<:"|"<%"|">:"|"#"|"@"

%%

{KEYWORDS1}	{ checkKeywords(yytext); printf(" <%6d,  %s  > \n", nextToken, yytext); }
{KEYWORDS2}	{ checkKeywords(yytext); printf(" <%6d,  %s  > \n", nextToken, yytext); }
{KEYWORDS3}	{ checkKeywords(yytext); printf(" <%6d,  %s  > \n", nextToken, yytext); }

[a-zA-Z][a-zA-Z_0-9]* | //标识符
[a-zA-Z][a-zA-Z0-9]*"_"[~!@#$%^&|*+-<>:?=/][~!@#$%^&|*+-<>:?=/]* | 
"`"(.*)"`"	{ nextToken = ID; printf(" <%6d,  %s  > \n", nextToken, yytext); }

{INTEGER} 	{ nextToken = INT; printf(" <%6d,  %s  > \n", nextToken, yytext); }

{FLOAT} 	{ nextToken = FLO; printf(" <%6d,  %s  > \n", nextToken, yytext); }

{STRING} 	{ nextToken = STR; printf(" <%6d,  %s  > \n", nextToken, yytext); }

{SYMBOL}	{ nextToken = SYM; printf(" <%6d,  %s  > \n", nextToken, yytext); }

{OPERATER}	{ nextToken = OPE; printf(" <%6d,  %s  > \n", nextToken, yytext); }

{B_COMMENT} { /*忽略注释*/ }
{L_COMMENT} { /*忽略注释*/ }

[ \n\t] { /*忽略空格和换行符*/ }

. { printf("Unknow: %s\n", yytext); }

%%


int main(int argc, char *argv[]){
	if (argc < 2)
	{
		printf("ERROR: input file name is needed. \n");
		exit(0);
	}
	yyin = fopen(argv[1], "r");
	if (yyin == NULL)
	{
		printf("ERROR:can not open file.\n");
		exit(0);
	}
	
	yylex();
	
}

int yywrap(){
	return 1;
}

