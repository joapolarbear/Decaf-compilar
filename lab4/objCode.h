#ifndef _OBJCODE_H_
#define _OBJCODE_H_

#define REGNUM 18
#define ALLREG 32
#define FSIZE 8	
#define TRUE 1
#define FALSE 0

typedef struct VarDes_t{
	char op[5];
	int regNo;
	int offset;
	struct VarDes_t* next;
} VarDes;

typedef struct RegDes_t{
	char name[3];
	VarDes* var;
	int old;
} RegDes;

void clearVarList();
void addVarToList(VarDes* p);
void stVar(VarDes* var, FILE* fp);
void ldVar(VarDes* var, FILE* fp);
void initAllRegDes();
void rstAllReg();
void freeOneReg(int regNo);
int allocateRegForOp(char* op, FILE* fp);
int getReg(FILE* fp);
void fwriteAllObjCode(char* fileName);
void fwriteOneObjCode(CodeLine* ir, FILE* fp);

#endif
