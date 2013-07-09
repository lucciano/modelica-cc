
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
#define IS_CREF(X) ((X)->expressionType()==EXPCOMPREF)
#define IS_UMINUS(X) ((X)->expressionType()==EXPUMINUS)
#define IS_UMINUS_VAR(X) (IS_UMINUS(X) && IS_CREF((X)->getAsUMinus()->exp()))
#define UMINUS_EXP(X) ((X)->getAsUMinus()->exp())
#define CREF_NAME(X) ( IS_UMINUS(X) ?  UMINUS_EXP(X)->getAsComponentReference()->name() : (X)->getAsComponentReference()->name() )
#define IS_VAR(X) ((IS_CREF(X) || IS_UMINUS_VAR(X)) && (!IS_PARAMETER(X)))
#define IS_ZERO_REAL(X) ((X)->expressionType()==EXPREAL && (X)->getAsReal()->val()==0.0)
#define IS_ZERO_INT(X) ((X)->expressionType()==EXPINTEGER && (X)->getAsInteger()->val()==0)
#define IS_ZERO(X) (IS_ZERO_REAL(X) || IS_ZERO_INT(X))
#define IS_ADD(X) ((X)->expressionType()==EXPBINOP && (X)->getAsBinOp()->binopType()==BINOPADD)
#define LEFT_EXP(X) ((X)->getAsBinOp()->left())
#define RIGHT_EXP(X) ((X)->getAsBinOp()->right())
#define IS_SUB(X) ((X)->expressionType()==EXPBINOP && (X)->getAsBinOp()->binopType()==BINOPSUB)
#define IS_SUM_(X) (IS_SUB(X) || IS_ADD(X))
#define IS_SUM_OF_VARS(X) (IS_SUM_(X) && (IS_VAR((X)->getAsBinOp()->left()) && IS_VAR((X)->getAsBinOp()->right())))
#define IS_STATE(X) (_varSymbolTable->lookup(CREF_NAME(X))!=NULL && _varSymbolTable->lookup(CREF_NAME(X))->isState())
#define IS_PARAMETER(X) (IS_UMINUS(X) ? _varSymbolTable->lookup(CREF_NAME(UMINUS_EXP(X)))!=NULL && _varSymbolTable->lookup(CREF_NAME(UMINUS_EXP(X)))->isParameter() :  _varSymbolTable->lookup(CREF_NAME(X))!=NULL && _varSymbolTable->lookup(CREF_NAME(X))->isParameter())

#define _VAR(v)			newAST_Expression_ComponentReferenceExp(v)
#define GREATER(l,r) 	newAST_Expression_BinOp(l, r, BINOPGREATER )
#define LOWER(l,r) 		newAST_Expression_BinOp(l, r, BINOPLOWER )
#define ADD(l,r) 		newAST_Expression_BinOp(l, r, BINOPADD )
#define MULT(l,r) 		newAST_Expression_BinOp(l, r, BINOPMULT )
#define SUB(l,r) 		newAST_Expression_BinOp(l, r, BINOPSUB )
#define I(n) 			newAST_Expression_Integer(n)
#define _R(n) 			newAST_Expression_Real(n)
#define _PA(e)       	newAST_Expression_OutputExpressions(e)
#define UMENOS(e)   	SUB( I(1) , e )
#define UNARYM(e)		newAST_Expression_UnaryMinus(e)
#define GREATEREQ(l,r) 	newAST_Expression_BinOp(l, r, BINOPGREATEREQ )
#define LOWEREQ(l,r) 	newAST_Expression_BinOp(l, r, BINOPLOWERWQ )
#define EQUAL(l,r)	 	newAST_Expression_BinOp(l, r,  BINOPCOMPEQ)


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
    case EXPOUTPUT:
    {
      AST_Expression_Output o = e->getAsOutput();
      return foldTraverse(AST_ListFirst(o->expressionList()));
    }
    case EXPUMINUS:
    {
      AST_Expression_UMinus u = e->getAsUMinus();
      return foldTraverseElementUMinus(u);
    }
    default:
      return foldTraverseElement(e);
    }
  };
private:
  virtual R foldTraverseElement(AST_Expression) = 0;
  virtual R foldTraverseElementUMinus(AST_Expression) = 0;
  virtual R foldTraverseElement(R, R, BinOpType) = 0;

};

class EqualExp {
public:
  static bool equalTraverse(AST_Expression a, AST_Expression b);
private:
  static bool equalTraverseElement(AST_Expression a, AST_Expression b);
};

class IsConstant: public AST_Expression_Fold<bool> {
public:
	IsConstant(VarSymbolTable st): _st(st) {};
private:
	virtual bool foldTraverseElement(AST_Expression);
	virtual bool foldTraverseElement(bool , bool , BinOpType);
	virtual bool foldTraverseElementUMinus(AST_Expression);
	VarSymbolTable _st;
};

/*
class ReplaceExp: public AST_Expression_Traverse  {
public:
	AST_Expression replaceExp(AST_Expression rep, AST_Expression for_exp, AST_Expression in);
private:
	virtual AST_Expression mapTraverseElement(AST_Expression);
	AST_Expression _rep, _for_exp, _in;
};
*/

/*  ReplaceBoolean: Reemplaza constantes Booleanas 
 *  Utilizado en las Modifications 
 * 
 */

class ReplaceBoolean: public AST_Expression_Fold<AST_Expression> {
public:
	ReplaceBoolean();
private:
	virtual AST_Expression foldTraverseElement(AST_Expression);
	virtual AST_Expression foldTraverseElement(AST_Expression , AST_Expression , BinOpType);
	virtual AST_Expression foldTraverseElementUMinus(AST_Expression);
};


/* WhenEqualityTrasforms: Realiza los cambios necesarios a los 
 * Equation_When antes de transformarlos a Statement 
 * 
 */


class WhenEqualityTrasforms: public AST_Expression_Fold<AST_Expression> {
public:
	WhenEqualityTrasforms();
private:
	virtual AST_Expression foldTraverseElement(AST_Expression);
	virtual AST_Expression foldTraverseElement(AST_Expression , AST_Expression , BinOpType);
	virtual AST_Expression foldTraverseElementUMinus(AST_Expression);
};

/* PreChange: Aplica la funcion Pre a la ComponentReference necesarias.
 * segun un conjunto pasado como parametro
 * 
 */ 

class PreChange: public AST_Expression_Fold<AST_Expression> {
public:
	PreChange(PreSet);
private:
	PreSet _pre;
	virtual AST_Expression foldTraverseElement(AST_Expression);
	virtual AST_Expression foldTraverseElement(AST_Expression , AST_Expression , BinOpType);
	virtual AST_Expression foldTraverseElementUMinus(AST_Expression);
};

/* FindReference: Devuelve si hay referencia de una variable en una expresion
 * 
 */

class FindReference: public AST_Expression_Fold<bool> {
public:
	FindReference(AST_String);
private:
	AST_String _var;
	virtual bool foldTraverseElement(AST_Expression);
	virtual bool foldTraverseElement(bool , bool , BinOpType);
	virtual bool foldTraverseElementUMinus(AST_Expression);
};

/* ReplaceReference: Cambia las referencias de una variable por otra
 * 
 * 
 */ 

class ReplaceReference: public AST_Expression_Fold<AST_Expression> {
public:
	ReplaceReference(AST_String,AST_String);
private:
	AST_String _pre;
	AST_String _post;
	virtual AST_Expression foldTraverseElement(AST_Expression);
	virtual AST_Expression foldTraverseElement(AST_Expression , AST_Expression , BinOpType);
	virtual AST_Expression foldTraverseElementUMinus(AST_Expression);
};


#endif
