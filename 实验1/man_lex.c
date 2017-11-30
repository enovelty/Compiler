/*	李昂 2016220304005 
*	关键字编号从300开始，标识符（ID）编号258，字符串261 
*	注释不输出 
*
*/


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h> 

int charClass;
#define MAX_LEN 1000
char lexeme[MAX_LEN];
char nextChar;
char next2Char;
int lexLen;
int token;
int nextToken;
FILE *inFile;

#define LETTER 0
#define DIGIT 1
#define POINT 2
#define UNKNOWN 999

enum {ID = 258, INT, FLOAT, STRING, ANNO, EQU, GEQU, LEQU, AND, OR, INVERT, ERROR};	 
		
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

void addChar()
{
	if (lexLen <= MAX_LEN -2)
	{
		lexeme[lexLen++] = nextChar;
		lexeme[lexLen] = 0;
	}
	else
		printf("ERROR:lexeme is too long. \n");
}

void getChar()
{
	static int firstRun = 1;
	if (firstRun)
	{
		nextChar = getc(inFile);
		next2Char = getc(inFile);
		firstRun = 0;
	}
	else
	{
		nextChar = next2Char;
		next2Char = getc(inFile);
	}
	
	if(nextChar == EOF)
	{
		charClass = EOF;
	}
	else
	{
		if (isalpha(nextChar))
			charClass = LETTER;
		else if (isdigit(nextChar))
			charClass = DIGIT;
		else if (nextChar == '.')
			charClass = POINT;
		else
			charClass = UNKNOWN;
	}
}

//跳过空白 
void getNonBlank()
{
	while (isspace(nextChar))
		getChar();
}

//处理字符串 
void pstring(){
	addChar();
	nextToken = nextChar;
	printf(" <%6d,  %s  > \n", nextToken, lexeme);	//输出字符串开头的 “ 
	lexLen = 0;
	while(next2Char != '\"' || (next2Char == '\"' && nextChar == '\\'))
	{
		getChar();
		addChar();
	}
	nextToken = STRING;
	printf(" <%6d,  %s  > \n", nextToken, lexeme);	//输出字符串 
	lexLen = 0;
	getChar();
	addChar();
	nextToken = nextChar;	//处理结尾的 ” 
}

//处理注释
void anno(){
	if (next2Char == '/'){	//处理 //型 注释 
		while (nextChar != '\n'){
			getChar();	
		}
	} 
	else{					//处理 /*型 注释 
		while (nextChar != '*' || next2Char != '/'){
			getChar();
		}
		getChar();
	}
	nextToken = ANNO; 
} 

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

int checkSymbol(char ch, char nextCh)
{
	switch (ch)
	{							//直接输出的符号 
	case'(': case')': case'{': case'}': case'[': case']':
	case'+': case'-': case'*': case';': case'.':
		addChar();
		nextToken = ch;
		break;
	case'"':					// "
		pstring();	//进入字符串处理 
		break;
	case'/':					// / 
		if (nextCh == '*' || nextCh == '/'){
			anno();	//进入注释处理
			break;	
		}	 
		addChar();
		nextToken = ch;
		break;
	case'=':					// =
		addChar();
		if (nextCh == '>')		//检查关键字 => 
		{
			getChar();
			addChar();
			checkKeywords("=>");
		}
		else if (nextCh == '=')
		{
			getChar();
			addChar();
			nextToken = EQU;
		} 
		else{					//检查关键字 =
			checkKeywords("=");
		}
		break;
	case'>':					// >
		addChar();
		if (nextCh == '=')
		{
			getChar();
			addChar();
			nextToken = GEQU;
		}
		else if (nextCh == ':')//检查关键字 >: 
		{
			getChar();
			addChar();
			checkKeywords(">:");
			break;
		}
		nextToken = ch;
		break;
	case'<':					// <
		addChar();
		if (nextCh == '=')
		{
			getChar();
			addChar();
			nextToken = LEQU;
			break;
		}
		else if (nextCh == '-')//检查关键字 <- 
		{
			getChar();
			addChar();
			checkKeywords("<-"); 
			break;
		}
		else if (nextCh == ':')//检查关键字 <:
		{
			getChar();
			addChar();
			checkKeywords("<:");
			break;
		}
		else if (nextCh == '%')//检查关键字 <%
		{
			getChar();
			addChar();
			checkKeywords("<%");
			break;
		}
		nextToken = ch;
		break;
	case'&':				  // &
		addChar();
		nextToken = ch;
		if (nextCh == '&')
		{
			getChar();
			addChar();
			nextToken = AND;
		}
		break;
	case'|':				 // |
		addChar();
		nextToken = ch;
		if (nextCh == '|')
		{
			getChar();
			addChar();
			nextToken = OR;
		}
		break;
	case'!':				// !
		addChar();
		nextToken = INVERT;
		break;
	case'_':				//检查关键字 _
		addChar();
		checkKeywords("_");
		break;
	case':':				//检查关键字 :
		addChar();
		checkKeywords(":");
		break;
	case'#':				//检查关键字 #
		addChar();
		checkKeywords("#");
		break;
	case'@':				//检查关键字 @
		addChar();
		checkKeywords("@");
		break;	
	case EOF:				//EOF
		addChar();
		nextToken = EOF;
	default:				//default
		printf("ERROR:unknown character'%c' \n",ch);
		nextToken = ERROR;
	}
	return nextToken;
}

//处理含有运算符的标识符 
int isSymbol(char ch){
	switch(ch)
	{	// ~!@#$%^&|*+-<>:?=/  18个操作符 和 _
		case '+': case '-': case '*': case '#':
		case ':': case '@': case '&': case '|':
		case '_': case '=': case '>': case '<':
		case '!': case '/': case '?': case '%':
		case '^': case '$': case '~':
			return 1;
			break;
		default: 
			return 0;
			break;
	}
}

int lexer()
{
	lexLen = 0;
	getNonBlank();
	switch (charClass)
	{
	case LETTER:
		addChar();
		getChar();
		while (charClass == LETTER || charClass == DIGIT || isSymbol(nextChar))
		{
			addChar();
			getChar();
		}
		nextToken = ID;
		//检查当前标识符是否是关键字
		checkKeywords(lexeme);
		break;
	case DIGIT:
 		addChar();
		getChar();
		int isfloat = 0;
		while (charClass == DIGIT || charClass == POINT)
		{
			if (charClass == POINT){
				isfloat = 1;
			}
			addChar();
			getChar();
		}
		if (isfloat)
			nextToken = FLOAT;
		else
			nextToken = INT;
		break;
	case UNKNOWN: case POINT:
		checkSymbol(nextChar, next2Char);
		getChar();
		break;
	case EOF:
		nextToken = EOF;
		lexeme[0] = 'E';
		lexeme[1] = 'O';
		lexeme[2] = 'F';
		lexeme[3] = 0;
	 	break; 
	}
	if (nextToken != ANNO)	//注释不输出 
		printf(" <%6d,  %s  > \n", nextToken, lexeme);
	return nextToken;
}

int main (int argc, char *argv[])
{
	if (argc < 2)
	{
		printf("ERROR: input file name is needed. \n");
		exit(0);
	}
	inFile = fopen(argv[1], "r");
	if (inFile == NULL)
	{
		printf("ERROR:can not open file.\n");
		exit(0);
	}
	getChar();
	while (nextToken != EOF)
		lexer();
}
