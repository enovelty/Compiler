/*
*	生成抽象语法树的递归下降分析程序
*	对于由多种语法子成分构成的语法成分，对相应的语法子成分调用函数，返回一个结点；
*	将结点、运算符等用函数封装为新结点，返回函数。（函数内部并不创建新结点，仅有声明）
*	单一语法结点则创建结点。
*	处理方式：对于文法中的终结符，如果正确匹配则 advance, 否则报错。
*	根据各个非终结符的 first 集，决定选择哪个产生式及出错处理。
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum yytype{ ID = 258, NUMBER, STRING, IF, ELSE, WHILE, RETURN, INT, STR, VOID,
			 PRINT, SCAN, CMP, ASSIGN};

extern int yylex();
extern FILE *yyin;
extern int yylval;
extern char *sval;
extern int lineno;

typedef struct _ast ast;
typedef struct _ast *past;

struct _ast{
	char *nodeType;
	int line;
	int value;
	char *str;
	past l;
	past r;
	past next;
};

int tok;

past program();
past external_declaration();
past decl_or_stmt();
past declarator_list();
past intstr_list();
past initializer();
past declarator();
past parameter_list();
past parameter();
past type();
past statement();
past statement_list();
past expression_statement();
past expr();
past cmp_expr();
past add_expr();
past mul_expr();
past primary_expr();
past expr_list();
past id_list();


void advance(){
	tok = yylex();
}

past newAstNode(){
	past p = (past)malloc(sizeof(ast));
	if (p == NULL){
		printf("New astNode failed.\n");
		exit(0);
	}
	memset(p, 0, sizeof(ast));
	p->line = lineno;
	p->l = NULL;
	p->r = NULL;
	p->next = NULL;
	return p;
}

//针对左递归文法
past newList(past list, past node){
	if (list != NULL){
		list->r->next = node;	//将新结点添加到链尾
		list->r = node;			//尾指针移动到链尾
		list->value += 1;
		return list;
	}
	//创建一个list
	list = newAstNode();
	list->next = node;
	list->value = 1;
	list->l = node;
	list->r = node;
	return list;
}

past newID(){
	past p = newAstNode();
	p->nodeType = "ID";
	p->value = tok;
	p->str = sval;
	//此处不能用yytext，个人理解：yytext是全局变量，这样做会导致yytext以后读取的字符串都继续赋值到str中
	return p;
}

past newNUMBER(){
	past p = newAstNode();
	p->nodeType = "NUMBER";
	p->value = yylval;
	return p;
}

past newSTRING(){
	past p = newAstNode();
	p->nodeType = "STRING";
	p->str = sval;
	return p;
}

past ifStmt(past cond, past trueStmt, past falseStmt){
	past p = newAstNode();
	p->nodeType = "ifStmt";
	p->l = trueStmt;
	p->r = falseStmt;
	p->next = cond;
	return p;
}

past assignStmt(int n, past l, past r){
	past p = newAstNode();
	p->nodeType = "assignStmt";
	p->value = n;
	p->str = sval;
	p->l = l;
	p->r = r;
	return p;
}

past array(past name, past expr){
	past p = newAstNode();
	p->nodeType = "array";
	p->l = name;
	p->r = expr;
	return p;
}

past newDecl(past type, past decl){
	past p = newAstNode();
	p->nodeType = "Decl";
	p->l = type;
	p->r = decl;
	return p;
}

past newStmt(past stmt_list){
	past p = newAstNode();
	p->nodeType = "Stmt";
	p->l = stmt_list;
	return p;
}

past whileStmt(past cond, past stmt){
	past p = newAstNode();
	p->nodeType = "whileStmt";
	p->l = cond;
	p->r = stmt;
	return p;
}

past returnStmt(past expr){
	past p = newAstNode();
	p->nodeType = "returnStmt";
	p->l = expr;
	return p;
}

past printStmt(past expr_list){
	past p = newAstNode();
	p->nodeType = "printStmt";
	p->l = expr_list;
	return p;
}

past scanStmt(past id_list){
	past p = newAstNode();
	p->nodeType = "scanStmt";
	p->l = id_list;
	return p;
}

past newExpr(int oper, past l, past r){
	past p = newAstNode();
	p->nodeType = "expr";
	p->value = oper;	//因为调用函数时当前tok可能已经不是oper对应的符号，所以不用str记录
	p->l = l;
	p->r = r;
	return p;
}


past extDecl(past t, past d, past ds){
	past p = newAstNode();
	p->nodeType = "extDecl";
	p->l = t;
	p->r = d;
	p->str = (char *)ds; //如果用next保存结点，输出语法树时不同的extDecl会混合
	return p;
}

past funcDecl(past direcDecl, past paraList){
	past p = newAstNode();
	p->nodeType = "funcDecl";
	p->l = direcDecl;
	p->r = paraList;
	return p;
}

past program(){
	advance();
	past ext = external_declaration();
	past list = newList(NULL, ext);
	while(tok){
		ext = external_declaration();
		list = newList(list, ext);
	}
	list->nodeType = "program";
	return list;
}

past external_declaration(){
	past t = type();
	past d = declarator();
	past ds = decl_or_stmt();

	past p = extDecl(t, d, ds);
	return p;
}

past decl_or_stmt(){
	past p = NULL;
	if(tok == '{'){
		advance();
		past stmt_l = NULL;
		if( tok == INT || tok == STR || tok == VOID || tok == '{' ||
			tok == ';' || tok == '-' || tok == '(' || tok == ID ||
			tok == NUMBER || tok == STRING ||tok == IF ||
			tok == WHILE || tok == RETURN ||tok == PRINT || tok == SCAN){
			stmt_l = statement_list();
		}
		if(tok == '}'){
			p = newStmt(stmt_l);
			advance();
		}
		else{
			printf("ERROR: line %d expect '}' --decl_or_stmt \n", lineno - 1);
		}
	}

	else if(tok == ','){
		advance();
		past decl_l = NULL;
		if(tok == ID){
			decl_l = declarator_list();
		}
		if(tok == ';'){
			p = decl_l;
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
	return p;
}

past declarator_list(){
	past p = newList(NULL, declarator());
	while(tok == ','){
		advance();
		p = newList(p, declarator());
	}
	p->nodeType = "decl_list";
	return p;
}

past intstr_list(){
	past p = newList(NULL, initializer());
	while(tok == ','){
		advance();
		p = newList(p, initializer());
	}
	p->nodeType = "instr_list";
	return p;
}

past initializer(){
	past p = NULL;
	if(tok == NUMBER){
		p = newNUMBER();
		advance();
	}
	else if(tok == STRING){
		p = newSTRING();
		advance();
	}
	else{
		printf("ERROR: line %d expect NUMBER or STRING --initializer\n", lineno);
	}
	return p;
}

past declarator(){
	past p = NULL;
	if (tok == ID){
		p = newID();
		advance();
	}
	else{
		printf("ERROR: line %d expect 'ID' --declarator\n", lineno);
	}

	if (tok == '='){//ID '=' expr
		int oper = tok;
		advance();
		past exp = expr();
		p = assignStmt(oper, p, exp);
	}

	else if(tok == '('){
		advance();
		past para = NULL;
		if(tok == INT || tok == STR || tok == VOID){
			para = parameter_list();
		}

		if(tok == ')'){
			p = funcDecl(p, para);
			advance();
		}
		else{
			printf("ERROR: line %d expect ')' --declarator\n", lineno - 1);
		}
	}

	else if(tok == '['){//match '['
		advance();
		if(tok == ']'){
			p = array(p, NULL);
			advance();
			if(tok == '='){//ID '[' ']' '=' '{' intstr_list '}'
				int oper = tok;
				advance();
				if(tok == '{'){
					advance();
				}
				else{
					printf("ERROR: line %d expect '{' --declarator\n", lineno);
				}
				past ini_l = intstr_list();
				if(tok == '}'){
					p = assignStmt(oper, p, ini_l);
					advance();
				}
				else{
					printf("ERROR: line %d expect '}' --declarator\n", lineno);
				}
			}//ID '[' ']' '=' '{' intstr_list '}'
		}
		else{//match 'expr'
			past exp = expr();
			if(tok == ']'){
				advance();
			}
			else{
				printf("ERROR: line %d expect ']' --declarator\n", lineno);
			}
			p = array(p, exp);

			if(tok == '='){//ID '[' expr ']' '=' '{' intstr_list '}'
				int oper = tok;
				advance();
				if(tok == '{'){
					advance();
				}
				else{
					printf("ERROR: line %d expect '{' --declarator\n", lineno);
				}
				past ini_l = intstr_list();
				if(tok == '}'){
					p = assignStmt(oper, p, ini_l);
					advance();
				}
				else{
					printf("ERROR: line %d expect '}' --declarator\n", lineno);
				}
			}//ID '[' expr ']' '=' '{' intstr_list '}'
		}//match 'expr'
	}//match '['
	return p;
//declarator 可以只产生一个ID，此处无需报错
}

past parameter_list(){
	past p = newList(NULL, parameter());
	while(tok == ','){
		advance();
		p = newList(p, parameter());
	}
	p->nodeType = "para_list";
	return p;
}

past parameter(){
	past p = newAstNode();
	p->nodeType = "parameter";
	p->l = type();

	if(tok == ID){
		p->r = newID();
		advance();
		return p;
	}
	else{
		printf("ERROR: line %d expect ID --parameter\n", lineno);
		return NULL;
	}
}

past type(){
	if(tok == INT || tok == STR || tok ==  VOID){
		past p = newAstNode();
		p->nodeType = "type";
		p->value = tok;
		advance();
		return p;
	}
	else{
		printf("ERROR: line %d type match failed\n", lineno);
		return NULL;
	}
}

past statement(){
	past p = NULL;
	if(tok == INT || tok == STR || tok ==  VOID){//type declarator_list ';'
		past l = type();
		past decl = declarator_list();
		if(tok == ';'){
			p = newDecl(l, decl);
			advance();
		}
		else{
			printf("ERROR: line %d expect ';' --statement type declarator_list\n", lineno - 1);
		}
	}

	else if(tok == '{'){//'{' statement_list '}'
		advance();
		p = statement_list();
		if(tok == '}'){
			advance();
		}
		else{
			printf("ERROR: line %d expect '}' --statement '{' statement_list '}'\n", lineno);
		}
	}

	else if(tok == ';' || tok == '-' || tok == '(' ||
			tok == ID || tok == NUMBER || tok == STRING){//expression_statement
		p = expression_statement();
	}

	else if(tok == IF){
		advance();
		if(tok == '('){
			advance();
		}
		else{
			printf("ERROR: line %d expect '(' --statemetnt IF\n", lineno);
		}
		past cond = expr();
		if(tok == ')'){
			advance();
		}
		else{
			printf("ERROR: line %d expect ')' --statement IF\n", lineno - 1);
		}
		past trueStmt = statement();
		past falseStmt = NULL;
		if(tok == ELSE){//'(' expr ')' statement ELSE statement
			advance();
			falseStmt = statement();
		}
		p = ifStmt(cond, trueStmt, falseStmt);
	}

	else if(tok == WHILE){//WHILE '(' expr ')' statement
		advance();
		if(tok == '('){
			advance();
		}
		else{
			printf("ERROR: line %d expect '(' --statement WHILE\n", lineno);
		}
		past cond = expr();
		if(tok == ')'){
			advance();
		}
		else{
			printf("ERROR: line %d expect ')' --statement WHILE\n", lineno - 1);
		}
		past stmt = statement();
		p = whileStmt(cond, stmt);
	}

	else if(tok == RETURN){
		advance();
		past exp = NULL;
		if(tok == ';'){
			advance();
		}
		else{
			exp = expr();
			if(tok == ';'){
				advance();
			}
			else{
				printf("ERROR: line %d expect ';' --statement RETURN\n", lineno - 1);
			}
		}
		p = returnStmt(exp);
	}

	else if(tok == PRINT){
		advance();
		past expr_l = NULL;
		if(tok == ';'){
			advance();
		}
		else{
			expr_l = expr_list();
			if(tok == ';'){
				advance();
			}
			else{
				printf("ERROR: line %d expect ';' --statement PRINT\n", lineno - 1);
			}
		}
		p = printStmt(expr_l);
	}

	else if(tok == SCAN){
		advance();
		past id_l = id_list();
		if(tok == ';'){
			advance();
			p = scanStmt(id_l);
		}
		else{
			printf("ERROR: line %d expect ';' --statement SCAN\n", lineno - 1);
		}
	}

	else{
		printf("ERROR: line %d statement match failed\n", lineno - 1);
	}
	return p;
}

past statement_list(){
	past p = newList(NULL, statement());
	while(tok != '}'){//文法中两次调用statement_list 均以'}'为结尾
		p = newList(p, statement());
	}
	p->nodeType = "stmt_list";
	return p;
}

past expression_statement(){
	past p = NULL;
	if(tok == ';'){
		advance();
	}
	else{
		p = expr();
		if(tok == ';'){
			advance();
		}
		else{
			printf("ERROR: line %d expect ';' --expression_statement\n", lineno - 1);
		}
	}
	return p;
}

past expr(){
	past p = cmp_expr();
	//此处不更改nodetype，因primary_expr函数返回的结点已经赋值。
	return p;
}

past cmp_expr(){
	past l = add_expr();
	while(tok == CMP){
		int oper = yylval;
		advance();
		past r = add_expr();
		l = newExpr(oper, l, r);
	}
	return l;
}

past add_expr(){
	past l = mul_expr();
	while(tok == '+' || tok == '-'){
		int oper = tok;
		advance();
		past r = mul_expr();
		l = newExpr(oper, l, r);
	}
	return l;
}

past mul_expr(){
	past l = NULL;
	if(tok == '-'){
		int oper = tok;
		advance();
		l = primary_expr();
		l = newExpr(oper, l, NULL);
	}
	else{
		l = primary_expr();
		while(tok == '*' || tok == '/' || tok == '%'){
			int oper = tok;
			advance();
			past r = primary_expr();
			l = newExpr(oper, l, r);
		}
	}
	return l;
}

past primary_expr(){
	past p = NULL;
	if(tok == ID){
		past id = newID();
		advance();
		if(tok == '('){
			advance();
			past el = NULL;
			if( tok == '-' || tok == '(' ||
				tok == ID || tok == NUMBER || tok == STRING){
				el = expr_list();
			}
			if(tok == ')'){
				advance();
				p = funcDecl(id, el);
				return p;
			}
			else{
				printf("ERROR: line %d expect ')' --primary_expr ID\n", lineno);
			}
		}
		else if(tok == ASSIGN){
			int oper = yylval;
			advance();
			past r = expr();
			p = assignStmt(oper, id, r);
			return p;
		}
		else if(tok == '='){
			int oper = tok;
			advance();
			past r = expr();
			p = assignStmt(oper, id, r);
			return p;
		}
		else if(tok == '['){
			advance();
			past r = expr();
			if(tok == ']'){
				advance();
				p = array(id, r);
				if(tok == '='){//ID '[' expr ']' '=' expr
					int oper = tok;
					advance();
					past r = expr();
					p = assignStmt(oper, p, r);
				}
				return p;
			}
			else{
				printf("ERROR: line %d expect ']' --ID '[' expr ']'\n", lineno);
			}
		}
		return id;
	}//ID

	else if(tok == '('){
		p = expr();
		if(tok == ')'){
			advance();
			return p;
		}
		else{
			printf("ERROR: line %d expect ')' --primary_expr '(' expr ')'\n", lineno);
		}
	}

	else if(tok == NUMBER){
		p = newNUMBER();
		advance();
		return p;
	}

	else if(tok == STRING){
		p = newSTRING();
		advance();
		return p;
	}

	else{
		printf("ERROR: line %d primary_expr match failed\n", lineno);
	}
	return p;
}

past expr_list(){
	past p = newList(NULL, expr());
	while(tok == ','){
		advance();
		p = newList(p, expr());
	}
	p->nodeType = "expr_list";
	return p;
}

past id_list(){
	past p = newList(NULL, newID());
	advance();
	while(tok == ','){
		advance();
		p = newList(p, newID());
		advance();
	}
	p->nodeType = "id_list";
	return p;
}

void showAst(past p, int nest)
{
	if(p == NULL)
		return;

	int i = 0;
	for(i = 0; i < nest; i++)
		printf("  ");

	if(strcmp(p->nodeType, "ID") == 0)
		printf("id %s\n", p->str);
	else if(strcmp(p->nodeType, "NUMBER") == 0)
		printf("num %d\n", p->value);
	else if(strcmp(p->nodeType, "STRING") == 0)
		printf("str %s\n", p->str);
	else if(strcmp(p->nodeType, "ifStmt") == 0){
		printf("%s \n", p->nodeType);
		showAst(p->next, nest + 1);
		showAst(p->l, nest + 1);
		showAst(p->r, nest + 1);
	}
	else if(strcmp(p->nodeType, "assignStmt") == 0){
		if (p->value == '=')
			printf("%s =\n", p->nodeType);
		else
			printf("%s %d\n", p->nodeType, p->value);
		showAst(p->l, nest + 1);
		showAst(p->r, nest + 1);
	}
	else if(strcmp(p->nodeType, "array") == 0){
		printf("%s \n", p->nodeType);
		showAst(p->l, nest + 1);
		showAst(p->r, nest + 1);
	}
	else if(strcmp(p->nodeType, "Decl") == 0){
		printf("%s \n", p->nodeType);
		showAst(p->l, nest + 1);
		showAst(p->r, nest + 1);
	}
	else if(strcmp(p->nodeType, "Stmt") == 0){
		printf("%s \n", p->nodeType);
		showAst(p->l, nest + 1);
	}
	else if(strcmp(p->nodeType, "whileStmt") == 0){
		printf("%s \n", p->nodeType);
		showAst(p->l, nest + 1);
		showAst(p->r, nest + 1);
	}
	else if(strcmp(p->nodeType, "returnStmt") == 0){
		printf("%s \n", p->nodeType);
		showAst(p->l, nest + 1);
	}
	else if(strcmp(p->nodeType, "printStmt") == 0){
		printf("%s \n", p->nodeType);
		showAst(p->l, nest + 1);
	}
	else if(strcmp(p->nodeType, "scanStmt") == 0){
		printf("%s \n", p->nodeType);
		showAst(p->l, nest + 1);
	}
	else if(strcmp(p->nodeType, "expr") == 0){
		if (p->value < 258)//输出ASCII符号
			printf("%s %c\n", p->nodeType, (char)p->value);
		else {
			switch (p->value) {
				case 272: printf("%s <\n", p->nodeType); break;
				case 273: printf("%s >\n", p->nodeType); break;
				case 274: printf("%s ==\n", p->nodeType); break;
				case 275: printf("%s <=\n", p->nodeType); break;
				case 276: printf("%s >=\n", p->nodeType); break;
				case 277: printf("%s !=\n", p->nodeType); break;
				case 278: printf("%s +=\n", p->nodeType); break;
				case 279: printf("%s -=\n", p->nodeType); break;
				case 280: printf("%s *=\n", p->nodeType); break;
				case 281: printf("%s /=\n", p->nodeType); break;
				case 282: printf("%s %%=\n", p->nodeType); break;//输出'%'用  %%
			}
		}

		showAst(p->l, nest + 1);
		showAst(p->r, nest + 1);
	}
	else if(strcmp(p->nodeType, "extDecl") == 0){
		printf("ext_decl \n");
		showAst(p->l, nest + 1);
		showAst(p->r, nest + 1);
		showAst((past)p->str, nest + 1);
	}
	else if(strcmp(p->nodeType, "funcDecl") == 0){
		printf("%s \n", p->nodeType);
		showAst(p->l, nest + 1);
		showAst(p->r, nest + 1);
	}
	else if(strcmp(p->nodeType, "program") == 0){
		printf("%s \n", p->nodeType);
		past t = p->l;
		int i = 1;
		for (; i <= p->value; i++){
			showAst(t, nest + 1);
			t = t->next;
		}
	}
	else if(strcmp(p->nodeType, "decl_list") == 0){
		printf("%s \n", p->nodeType);
		past t = p->l;
		int i = 1;
		for (; i <= p->value; i++){
			showAst(t, nest);
			t = t->next;
		}
	}
	else if(strcmp(p->nodeType, "instr_list") == 0){
		printf("%s \n", p->nodeType);
		past t = p->l;
		int i = 1;
		for (; i <= p->value; i++){
			showAst(t, nest);
			t = t->next;
		}
	}
	else if(strcmp(p->nodeType, "para_list") == 0){
		printf("%s \n", p->nodeType);
		past t = p->l;
		int i = 1;
		for (; i <= p->value; i++){
			showAst(t, nest);
			t = t->next;
		}
	}
	else if(strcmp(p->nodeType, "parameter") == 0){
		printf("%s \n", p->nodeType);
		showAst(p->l, nest + 1);
		showAst(p->r, nest + 1);
	}
	else if(strcmp(p->nodeType, "type") == 0){
		if (p->value == INT)
			printf("%s int\n", p->nodeType);
		else if (p->value == STR)
			printf("%s str\n", p->nodeType);
		else if (p->value == VOID)
			printf("%s void\n", p->nodeType);
		else
			printf("%s \n", p->nodeType);
	}
	else if(strcmp(p->nodeType, "stmt_list") == 0){
		printf("%s \n", p->nodeType);
		past t = p->l;
		int i = 1;
		for (; i <= p->value; i++){
			showAst(t, nest);
			t = t->next;
		}
	}
	else if(strcmp(p->nodeType, "expr_list") == 0){
		printf("%s \n", p->nodeType);
		past t = p->l;
		int i = 1;
		for (; i <= p->value; i++){
			showAst(t, nest);
			t = t->next;
		}
	}
	else if(strcmp(p->nodeType, "id_list") == 0){
		printf("%s \n", p->nodeType);
		past t = p->l;
		int i = 1;
		for (; i <= p->value; i++){
			showAst(t, nest);
			t = t->next;
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

	past r = program();
	showAst(r, 0);

	return 0;
}
