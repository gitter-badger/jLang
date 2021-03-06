#include <iostream>
#include "llvm/Analysis/Passes.h"
#include "llvm/IR/Verifier.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Transforms/Scalar.h"
#include <cctype>
#include <cstdio>
#include <map>
#include <string>
#include <vector>
#include<stdio.h>
#include<stdlib.h>
#include<fstream>
#include "jlang.tab.hpp"
#include "lex.h" 
#include "tree.h"
using namespace std;
using namespace llvm;

void createExtern(PrototypeAST* P)
{
  Function* func = P->Codegen();
  if(!func)
  {
    cerr << "\033[31m ERROR: \033[37m Error in declaring extern " << endl;
    exit(EXIT_FAILURE);
  }
#ifdef DEBUG
  func->dump();
#endif
}

extern FILE* yyin;
extern int YY_FLUSH_BUFFER;
extern vector<ExprAST*>* lines;
extern YY_BUFFER_STATE getCurBuf();
extern Module *theModule;
extern SymbolTable<string, Value*> NamedValues;

void loadModule(const char* name)
{
  lines->clear();
  string modName(name);
  modName += ".jl";
  YY_BUFFER_STATE tmp = getCurBuf();
  FILE* newBuf;
  if(!(newBuf = fopen(modName.c_str(),"r")))
  {
    cerr << "\033[31m ERROR: \033[37m Can not read module "<< name << "!" << endl;
    exit(EXIT_FAILURE);
  }
  YY_BUFFER_STATE modFile = yy_create_buffer(newBuf,YY_BUF_SIZE);
  yy_switch_to_buffer(modFile);
  yyparse();
  vector<ExprAST*>::iterator it;
  Value* cur;
  for(it = lines->begin(); it != lines->end(); it++)
  {
    cur = (*it)->Codegen();
    if(!cur)
    {
      cerr << "\033[31m INTERNAL ERROR: \033[37m Error in reading AST in module " << name << endl;
      exit(EXIT_FAILURE);
    }
  }
  lines->clear();
  NamedValues.clear();
  yy_switch_to_buffer(tmp);
  yy_delete_buffer(modFile);
}
