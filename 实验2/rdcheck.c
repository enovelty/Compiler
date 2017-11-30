/*
*	不生成抽象语法树的递归下降分析程序
*	处理方式：对于文法中的终结符，如果正确匹配则 advance, 否则报错。
*	根据各个非终结符的 first 集，决定选择哪个产生式及出错处理。
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum yytype{ ID = 258, NUMBER, STRING, IF, ELSE, WHILE, RETURN, INT, STR, VOID,
			 PRINT, SCAN, CMP, ASSIGN};

extern int yylex();
extern char *yytext;
extern FILE *yyin;
extern int lineno;

int tok;

void program();
void external_declaration();
void decl_or_stmt();
void declarator_list();
void intstr_list();
void initializer();
void declarator();
void parameter_list();
void parameter();
void type();
void statement();
void statement_list();
void expression_statement();
void expr();
void cmp_expr();
void add_expr();
void mul_expr();
void primary_expr();
void expr_list();
void id_list();


void advance(){
	tok = yylex();
	printf("tok %3d : %s\n", tok, yytext);
	if(tok == 0){
		printf("Analyse complete.\n");
		exit(1);
	}
}

void program(){
	advance();
	external_declaration();
	while(tok){
		external_declaration();
	}
}

void external_declaration(){
	type();
	declarator();
	decl_or_stmt();
}

void decl_or_stmt(){
	if(tok == '{'){
		advance();
		if( tok == INT || tok == STR || tok == VOID || tok == '{' ||
			tok == ';' || tok == '-' || tok == '(' || tok == ID ||
			tok == NUMBER || tok == STRING ||tok == IF ||
			tok == WHILE || tok == RETURN ||tok == PRINT || tok == SCAN){
			statement_list();
		}
		if(tok == '}'){
			advance();
		}
		else{
			printf("ERROR: line %d expect '}' --decl_or_stmt \n", lineno - 1);
		}
	}

	else if(tok == ','){
		advance();
		if(tok == ID){
			declarator_list();
		}
		if(tok == ';'){
			advance();
		}
		else{
			printf("ERROR: line %d expect ';' --decl_or_stmt\n", lineno - 1);
		}
	}

	else if(tok == ';'){
		advance();
	}

	else{
		printf("ERROR: line %d decl_or_stmt match failed\n", lineno - 1);
	}
}

void declarator_list(){
	declarator();
	while(tok == ','){
		advance();
		declarator();
	}
}

void intstr_list(){
	initializer();
	while(tok == ','){
		advance();
		initializer();
	}
}

void initializer(){
	if(tok == NUMBER || tok == STRING){
		advance();
	}
	else{
		printf("ERROR: line %d expect NUMBER or STRING --initializer\n", lineno);
	}
}

void declarator(){
	if (tok == ID){
		advance();
	}
	else{
		printf("ERROR: line %d expect 'ID' --declarator\n", lineno);
	}

	if (tok == '='){//ID '=' expr
		advance();
		expr();
	}

	else if(tok == '('){
		advance();
		if(tok == INT || tok == STR || tok == VOID){
			parameter_list();
		}

		if(tok == ')'){
			advance();
		}
		else{
			printf("ERROR: line %d expect ')' --declarator\n", lineno - 1);
		}
	}

	else if(tok == '['){//match '['
		advance();
		if(tok == ']'){
			advance();
			if(tok == '='){//ID '[' ']' '=' '{' intstr_list '}'
				advance();
				if(tok == '{'){
					advance();
				}
				else{
					printf("ERROR: line %d expect '{' --declarator\n", lineno);
				}
				intstr_list();
				if(tok == '}'){
					advance();
				}
				else{
					printf("ERROR: line %d expect '}' --declarator\n", lineno);
				}
			}//ID '[' ']' '=' '{' intstr_list '}'
		}
		else{//match 'expr'
			expr();
			if(tok == ']'){
				advance();
			}
			else{
				printf("ERROR: line %d expect ']' --declarator\n", lineno);
			}

			if(tok == '='){//ID '[' expr ']' '=' '{' intstr_list '}'
				advance();
				if(tok == '{'){
					advance();
				}
				else{
					printf("ERROR: line %d expect '{' --declarator\n", lineno);
				}
				intstr_list();
				if(tok == '}'){
					advance();
				}
				else{
					printf("ERROR: line %d expect '}' --declarator\n", lineno);
				}
			}//ID '[' expr ']' '=' '{' intstr_list '}'
		}//match 'expr'
	}//match '['

//declarator 可以只产生一个ID，此处无需报错
}

void parameter_list(){
	parameter();
	while(tok == ','){
		advance();
		parameter();
	}
}

void parameter(){
	type();
	if(tok == ID){
		advance();
	}
	else{
		printf("ERROR: line %d expect ID --parameter\n", lineno);
	}
}

void type(){
	if(tok == INT || tok == STR || tok ==  VOID){
		advance();
	}
	else{
		printf("ERROR: line %d type match failed\n", lineno);
	}
}

void statement(){
	if(tok == INT || tok == STR || tok ==  VOID){//type declarator_list ';'
		advance();
		declarator_list();
		if(tok == ';'){
			advance();
		}
		else{
			printf("ERROR: line %d expect ';' --statement type declarator_list\n", lineno - 1);
		}
	}

	else if(tok == '{'){//'{' statement_list '}'
		advance();
		statement_list();
		if(tok == '}'){
			advance();
		}
		else{
			printf("ERROR: line %d expect '}' --statement '{' statement_list '}'\n", lineno);
		}
	}

	else if(tok == ';' || tok == '-' || tok == '(' ||
			tok == ID || tok == NUMBER || tok == STRING){//expression_statement
		expression_statement();
	}

	else if(tok == IF){
		advance();
		if(tok == '('){
			advance();
		}
		else{
			printf("ERROR: line %d expect '(' --statemetnt IF\n", lineno);
		}
		expr();
		if(tok == ')'){
			advance();
		}
		else{
			printf("ERROR: line %d expect ')' --statement IF\n", lineno - 1);
		}
		statement();
		if(tok == ELSE){//'(' expr ')' statement ELSE statement
			advance();
			statement();
		}
	}

	else if(tok == WHILE){//WHILE '(' expr ')' statement
		advance();
		if(tok == '('){
			advance();
		}
		else{
			printf("ERROR: line %d expect '(' --statement WHILE\n", lineno);
		}
		expr();
		if(tok == ')'){
			advance();
		}
		else{
			printf("ERROR: line %d expect ')' --statement WHILE\n", lineno - 1);
		}
		statement();
	}

	else if(tok == RETURN){
		advance();
		if(tok == ';'){
			advance();
		}
		else{
			expr();
			if(tok == ';'){
				advance();
			}
			else{
				printf("ERROR: line %d expect ';' --statement RETURN\n", lineno - 1);
			}
		}
	}

	else if(tok == PRINT){
		advance();
		if(tok == ';'){
			advance();
		}
		else{
			expr_list();
			if(tok == ';'){
				advance();
			}
			else{
				printf("ERROR: line %d expect ';' --statement PRINT\n", lineno - 1);
			}
		}
	}

	else if(tok == SCAN){
		advance();
		id_list();
		if(tok == ';'){
			advance();
		}
		else{
			printf("ERROR: line %d expect ';' --statement SCAN\n", lineno - 1);
		}
	}

	else{
		printf("ERROR: line %d statement match failed\n", lineno - 1);
	}

}

void statement_list(){
	while(tok != '}'){//文法中两次调用statement_list 均以'}'为结尾
		statement();
	}
}

void expression_statement(){
	if(tok == ';'){
		advance();
	}
	else{
		expr();
		if(tok == ';'){
			advance();
		}
		else{
			printf("ERROR: line %d expect ';' --expression_statement\n", lineno - 1);
		}
	}
}

void expr(){
	cmp_expr();
}

void cmp_expr(){
	add_expr();
	while(tok == CMP){
		advance();
		cmp_expr();
	}
}

void add_expr(){
	mul_expr();
	while(tok == '+' || tok == '-'){
		advance();
		mul_expr();
	}
}

void mul_expr(){
	if(tok == '-'){
		advance();
		primary_expr();
	}
	else{
		primary_expr();
		while(tok == '*' || tok == '/' || tok == '%'){
			advance();
			primary_expr();
		}
	}
}

void primary_expr(){
	if(tok == ID){
		advance();
		if(tok == '('){
			advance();
			if( tok == '-' || tok == '(' ||
				tok == ID || tok == NUMBER || tok == STRING){
				expr_list();
			}
			if(tok == ')'){
				advance();
			}
			else{
				printf("ERROR: line %d expect ')' --primary_expr ID\n", lineno);
			}
		}
		else if(tok == ASSIGN){
			advance();
			expr();
		}
		else if(tok == '='){
			advance();
			expr();
		}
		else if(tok == '['){
			advance();
			expr();
			if(tok == ']'){
				advance();
				if(tok == '='){//ID '[' expr ']' '=' expr
					advance();
					expr();
				}
			}
			else{
				printf("ERROR: line %d expect ']' --ID '[' expr ']'\n", lineno);
			}
		}
	}

	else if(tok == '('){
		expr();
		if(tok == ')'){
			advance();
		}
		else{
			printf("ERROR: line %d expect ')' --primary_expr '(' expr ')'\n", lineno);
		}
	}

	else if(tok == NUMBER){
		advance();
	}

	else if(tok == STRING){
		advance();
	}

	else{
		printf("ERROR: line %d primary_expr match failed\n", lineno);
	}
}

void expr_list(){
	expr();
	while(tok == ','){
		expr();
	}
}

void id_list(){
	if(tok == ID){
		advance();
	}
	else{
		printf("ERROR: line %d expect ID --id_list\n", lineno);
	}
	while(tok == ','){
		advance();
		if(tok == ID){
			advance();
		}
		else{
			printf("ERROR: line %d expect ID --id_list\n", lineno);
		}
	}
}

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		printf("ERROR: input file name is needed.\n");
		exit(0);
	}
	yyin = fopen(argv[1], "r");
	if (yyin == NULL)
	{
		printf("ERROR: can not open file.\n");
		exit(0);
	}

	program();

	return 0;
}
