
/*****************************************************************************

    This file is part of Modelica C Compiler.

    Modelica C Compiler is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Modelica C Compiler is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Modelica C Compiler.  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/

#include <ast/ast_builder.h>
#include <ast/expression.h>
#include <util/symbol_table.h>

#ifndef AST_UTIL 
#define AST_UTIL
#define CREF_NAME(X) ((X)->getAsComponentRef()->name())
#define IS_CREF(X) ((X)->expressionType()==EXPCOMPREF)
#define IS_UMINUS(X) ((X)->expressionType()==EXPUMINUS)
#define IS_UMINUS_VAR(X) (IS_UMINUS(X) && IS_CREF((X)->getAsUMinus()->exp()))
#define IS_VAR(X) (IS_CREF(X) || IS_UMINUS_VAR(X))
#define IS_ZERO_REAL(X) ((X)->expressionType()==EXPREAL && (X)->getAsReal()->val()==0.0)
#define IS_ZERO_INT(X) ((X)->expressionType()==EXPINTEGER && (X)->getAsInteger()->val()==0)
#define IS_ZERO(X) (IS_ZERO_REAL(X) || IS_ZERO_INT(X))
#define IS_ADD(X) ((X)->expressionType()==EXPBINOP && (X)->getAsBinOp()->binopType()==BINOPADD)
#define LEFT_EXP(X) ((X)->getAsBinOp()->left())
#define RIGHT_EXP(X) ((X)->getAsBinOp()->right())
#define IS_SUB(X) ((X)->expressionType()==EXPBINOP && (X)->getAsBinOp()->binopType()==BINOPSUB)
#define IS_SUM_(X) (IS_SUB(X) || IS_ADD(X))
#define IS_SUM_OF_VARS(X) (IS_SUM_(X) && (IS_VAR((X)->getAsBinOp()->left()) && IS_VAR((X)->getAsBinOp()->right())))
#define IS_STATE(X) (_varSymbolTable->lookup((X)->getAsComponentRef()->name())!=NULL && _varSymbolTable->lookup(X->getAsComponentRef()->name())->isState())



class AST_Expression_Traverse {
public:
  AST_Expression mapTraverse(AST_Expression);
private:
  virtual AST_Expression mapTraverseElement(AST_Expression) = 0;
};

template<class R>
class AST_Expression_Fold {
public:
  R foldTraverse(AST_Expression e) {
    switch (e->expressionType()) {
      case EXPBINOP:
        {
        AST_Expression_BinOp b = e->getAsBinOp();
        return foldTraverseElement (foldTraverse(b->left()),foldTraverse(b->right()),b->binopType());
        }
      default:
        return foldTraverseElement(e);
    }
  };
private:
  virtual R foldTraverseElement(AST_Expression) = 0;
  virtual R foldTraverseElement(R, R, BinOpType) = 0;

};

class EqualExp  {
public:
  static bool isEqual(AST_Expression, AST_Expression);
/*
private:
  virtual bool foldTraverseElement(AST_Expression);
  virtual bool foldTraverseElement(bool , bool , BinOpType);
*/
};

class IsConstant: public AST_Expression_Fold<bool> {
public:
   IsConstant(VarSymbolTable st): _st(st) {};
private:
  virtual bool foldTraverseElement(AST_Expression);
  virtual bool foldTraverseElement(bool , bool , BinOpType);
  VarSymbolTable _st;
};

class ReplaceExp: public AST_Expression_Traverse  {
  public:
    AST_Expression replaceExp(AST_Expression rep, AST_Expression for_exp, AST_Expression in);
private:
  virtual AST_Expression mapTraverseElement(AST_Expression);
  AST_Expression _rep, _for_exp, _in;
};

#endif
