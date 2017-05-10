#ifndef _THREEADDCODE_H
#define _THREEADDCODE_H

typedef struct CodeLine {
	char op[10];
	char place[3][20];
	struct CodeLine *next;
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
ThreeAddCode *CreateTAC(char *newOp, char *place1, char *place2, char *place3);
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