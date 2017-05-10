#ifndef _THREEADDCODE_H
#define _THREEADDCODE_H

#define LABEL_CODE		1
#define FUNCTION_CODE	2
#define ASSIGN_CODE 	3
#define ADD_CODE		4
#define SUB_CODE		5
#define MUL_CODE		6
#define DIV_CODE		7
#define ADDRESS_CODE	8
#define GOTO_CODE		9
#define IF_GOTO_CODE	10
#define RETURN_CODE		11
#define DEC_CODE		12
#define ARG_CODE		13
#define CALL_CODE		14
#define PARAM_CODE		15
#define READ_CODE		16
#define WRITE_CODE		17
#define MOD_CODE		18
#define LESS_CODE		19
#define MORE_CODE		20
#define EUQAL_CODE		21
#define LESSEQ_CODE		22
#define MOREEQ_CODE		23
#define AND_CODE		24
#define OR_CODE		    25
#define NOTEQ_CODE		26
#define NOT_CODE		27
#define MINUS_CODE		28

typedef struct CodeLine {
	char op[10];
	char place[3][20];
	int kind;
	struct CodeLine *next;
	struct CodeLine *pre;
}CodeLine;

typedef struct ThreeAddCode {
	struct CodeLine *head, *tail; 
}ThreeAddCode;

typedef struct parmList
{
	char t[5];
	struct parmList *next;
}ParmList;

ThreeAddCode *LinkTAC(ThreeAddCode *code1, ThreeAddCode *code2);
ThreeAddCode *CreateTAC(char *newOp, char *place1, char *place2, char *place3, int kind);
void CreateVarP(char *place);
void CreatePlace(char *place);
void CreateLable(char *l);
void itoa(int i,char*string);
void gen(struct ast *tree);
ThreeAddCode *translate_IfStmt(struct ast *a, char *l_out);
ThreeAddCode *translate_WhileStmt(struct ast *a, char *l_out);
ThreeAddCode *translate_ReturnStmt(struct ast *a);
ThreeAddCode *translate_PrintStmt(struct ast *a);
ThreeAddCode *translate_StmtBlock(struct ast *a, char *l_out);
ThreeAddCode *translate_Stmt(struct ast *a, char *l_out);
ThreeAddCode *translate_SimpleStmt(struct ast* a);
ThreeAddCode *translate_ForStmt(struct ast* a, char *l_out);
ThreeAddCode *translate_Exp(struct ast *a, char *t_in);
ThreeAddCode *translate_Lvalue(struct ast* a, char *t_in);
ThreeAddCode *translate_Parm(struct ast* a, ParmList **pph);
ThreeAddCode *translate_Call(struct ast* a, char *t_in);
ThreeAddCode *translate_ReadInteger(struct ast *a, char *t_in);
void printTAC();
ThreeAddCode *code;


#endif