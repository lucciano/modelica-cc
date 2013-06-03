
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

#include <util/ast_util.h>
#include <ast/ast_builder.h>
#include <ast/expression.h>

#include <iostream>
using namespace std;

AST_Expression AST_Expression_Traverse::mapTraverse(AST_Expression e) { 
  AST_Expression e2 = mapTraverseElement(e);
  switch (e2->expressionType()) {
    case EXPBINOP:
      {
        AST_Expression_BinOp b = e2->getAsBinOp();
        return newAST_Expression_BinOp(mapTraverse(b->left()),mapTraverse(b->right()), b->binopType());
      }
    case EXPBOOLEANNOT: 
      {
        AST_Expression_BooleanNot n = e2->getAsBooleanNot();
        return newAST_Expression_BooleanNot(mapTraverse(n->exp()));
      }

    case EXPUMINUS: 
      {
        AST_Expression_UMinus m = e2->getAsUMinus();
        return newAST_Expression_UnaryMinus(mapTraverse(m->exp()));
      }
    
    
    case EXPOUTPUT :
	{
		AST_Expression_Output b = e2->getAsOutput();
        AST_ExpressionList ls = new list < AST_Expression > ();
        AST_ListAppend(ls,mapTraverse( b->expressionList()->front() ))	;
        return newAST_Expression_OutputExpressions(ls);
	}  
      
    case EXPIF:
      {   
        AST_Expression_If i = e2->getAsIf();
        return newAST_Expression_If(mapTraverse(i->condition()), mapTraverse(i->then()), i->elseif_list(), mapTraverse(i->else_exp()));
      }
  }  
  return e2;
}

bool EqualExp::equalTraverse(AST_Expression a, AST_Expression b) {
  if (a->expressionType()!=b->expressionType()) return false;
  switch (a->expressionType()) {
    case EXPBINOP:
    {
      AST_Expression_BinOp binOpA = a->getAsBinOp();
      AST_Expression_BinOp binOpB = b->getAsBinOp();
      equalTraverse(binOpA->left(), binOpB->left()) && equalTraverse(binOpA->right(), binOpB->right());
    }
      break;
    default:
      return equalTraverseElement(a, b);
  }
  return false;
}

bool EqualExp::equalTraverseElement(AST_Expression a, AST_Expression b) {
  if (a->expressionType()!=b->expressionType()) return false;
  switch (a->expressionType()) {
    case EXPCOMPREF:
      return CREF_NAME(a)==CREF_NAME(b);
      break;
    case EXPDERIVATIVE:
      AST_Expression_ComponentReference compRef1 = a->getAsDerivative()->arguments()->front()->getAsComponentRef();
      AST_Expression_ComponentReference compRef2 = b->getAsDerivative()->arguments()->front()->getAsComponentRef();
      return CREF_NAME(compRef1)==CREF_NAME(compRef2);
      break;
      // TODO faltan casos a considerar.
  }
  return false;
}

bool IsConstant::foldTraverseElement(bool b1, bool b2, BinOpType ) {
  return b1 && b2;
}

bool IsConstant::foldTraverseElement(AST_Expression e) {
  switch (e->expressionType()) {
    case EXPREAL: 
    case EXPINTEGER: 
    case EXPSTRING: 
    case EXPBOOLEAN: 
      return true;
    case EXPCOMPREF:
      AST_Expression_ComponentReference cr = e->getAsComponentRef();
      VarInfo v = _st->lookup(cr->name());
      if (v->isParameter()) return true;
      // Check symbol table!!!
      return false;
  }
  return false;
};




AST_Expression ReplaceExp::replaceExp(AST_Expression rep, AST_Expression for_exp, AST_Expression in) {
  _rep=rep;
  _for_exp=for_exp;
  _in=in;
  return mapTraverse(in);
} 
AST_Expression ReplaceExp::mapTraverseElement(AST_Expression e) {
  if (EqualExp::equalTraverse(e,_rep))
    return _for_exp;
  return e;
}


ReplaceBoolean::ReplaceBoolean() {}

AST_Expression ReplaceBoolean::foldTraverseElement(AST_Expression b1, AST_Expression b2, BinOpType t ) {
  return newAST_Expression_BinOp(b1,b2,t);
}

AST_Expression ReplaceBoolean::foldTraverseElement(AST_Expression e) {
  switch (e->expressionType()) {  
    case EXPBOOLEAN: 
    {
		AST_Expression_Boolean b = e->getAsBoolean();
		if (b->value()) return newAST_Expression_Integer(1);
		else return newAST_Expression_Integer(0);
	}
	
	default:
	{
		return e;
	}
  }
};
