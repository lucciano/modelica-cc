
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
  case EXPCALL:
  {
    AST_Expression_Call c = e2->getAsCall();
    AST_ExpressionList args = newAST_ExpressionList();
    AST_ExpressionListIterator args_it;
    foreach(args_it,c->arguments()) {
      AST_ListAppend(args,mapTraverse(current_element(args_it)));
    }
    return newAST_Expression_Call(c->name(),newAST_StringNull(),args);
  }
  default:
    /* TODO */
    break;
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
  {
    return CREF_NAME(a)==CREF_NAME(b);
    break;
  }
  case EXPDERIVATIVE:
  {
    AST_Expression_ComponentReference compRef1 = a->getAsDerivative()->arguments()->front()->getAsComponentReference();
    AST_Expression_ComponentReference compRef2 = b->getAsDerivative()->arguments()->front()->getAsComponentReference();
    return CREF_NAME(compRef1)==CREF_NAME(compRef2);
    break;
  }
  // TODO faltan casos a considerar.
  default:
    break;
  }
  return false;
}

bool IsConstant::foldTraverseElement(bool b1, bool b2, BinOpType ) {
  return b1 && b2;
}

bool IsConstant::foldTraverseElementUMinus(AST_Expression e) {
  return foldTraverse(e->getAsUMinus()->exp()); 
}
bool IsConstant::foldTraverseElement(AST_Expression e) {
  switch (e->expressionType()) {
  case EXPREAL:
  case EXPINTEGER:
  case EXPSTRING:
  case EXPBOOLEAN:
    return true;
  case EXPCOMPREF:
  {
    AST_Expression_ComponentReference cr = e->getAsComponentReference();
    VarInfo v = _st->lookup(cr->name());
    if (v->isParameter()) return true;
    // Check symbol table!!!
    return false;
  }
  default:
    break;
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

AST_Expression ReplaceBoolean::foldTraverseElementUMinus(AST_Expression u) {
  return foldTraverse(u->getAsUMinus()->exp());
}

AST_Expression ReplaceBoolean::foldTraverseElement(AST_Expression e) {
  switch (e->expressionType()) {
  case EXPBOOLEAN:
  {
    AST_Expression_Boolean b = e->getAsBoolean();
    if (b->value()) return newAST_Expression_Real(1.0);
    else return newAST_Expression_Real(0.0);
  }

  default:
  {
    return e;
  }
  }
};

WhenEqualityTrasforms::WhenEqualityTrasforms() {}

AST_Expression WhenEqualityTrasforms::foldTraverseElement(AST_Expression b1, AST_Expression b2, BinOpType t ) {
  return newAST_Expression_BinOp(b1,b2,t);
}

AST_Expression WhenEqualityTrasforms::foldTraverseElementUMinus(AST_Expression u) {
   return newAST_Expression_UnaryMinus( foldTraverse(u->getAsUMinus()->exp()) );
}

AST_Expression WhenEqualityTrasforms::foldTraverseElement(AST_Expression e) {
  switch (e->expressionType()) {
  case EXPBOOLEAN:
  {
    AST_Expression_Boolean b = e->getAsBoolean();
    if (b->value()) return newAST_Expression_Real(1.0);
    else return newAST_Expression_Real(0.0);
  }

/*  case EXPUMINUS:
  {
    AST_Expression_UMinus u = e->getAsUMinus();
    return newAST_Expression_UnaryMinus( foldTraverse(u->exp()) );
  }
*/
  case EXPOUTPUT :
  {
    AST_Expression_Output b = e->getAsOutput();
    return newAST_Expression_OutputExpressions(newAST_SimpleList(foldTraverse(b->expressionList()->front())));
  }

  case EXPCALL:
  {
    AST_Expression_Call call = e->getAsCall();
    if (toStr(call->name())  == "edge") {
      return  GREATER(  call->arguments()->front()   , _R(0.5) ) ;
    }

    return call;
  }

  case EXPCOMPREF:
  {
    return e;
  }

  case EXPBOOLEANNOT:
  {
    AST_Expression_BooleanNot no = e->getAsBooleanNot();
    return newAST_Expression_BooleanNot(foldTraverse( no->exp())) ;
  }

  case EXPIF:
  {
    AST_Expression_If i = e->getAsIf();
    AST_Expression eq1 = foldTraverse(i->then());
    AST_Expression eq2 = foldTraverse(i->else_exp());
    AST_Expression cond = foldTraverse(i->condition());
    return newAST_Expression_If(cond,eq1,newAST_ExpressionList(),eq2);
  }

  default:
  {
    return e;
  }
  }
};

PreChange::PreChange(PreSet p) {
  _pre = p;
}

AST_Expression PreChange::foldTraverseElement(AST_Expression b1, AST_Expression b2, BinOpType t ) {
  return newAST_Expression_BinOp(b1,b2,t);
}


AST_Expression PreChange::foldTraverseElementUMinus(AST_Expression u) {
   return newAST_Expression_UnaryMinus( foldTraverse(u->getAsUMinus()->exp()) );
}

AST_Expression PreChange::foldTraverseElement(AST_Expression e) {
  switch (e->expressionType()) {
	  case EXPOUTPUT :
	  {
		AST_Expression_Output b = e->getAsOutput();
		return newAST_Expression_OutputExpressions(newAST_SimpleList(foldTraverse(b->expressionList()->front())));
	  }

	  case EXPCALL:
	  {
		AST_Expression_Call call = e->getAsCall();
		AST_ExpressionListIterator it;
		foreach(it , call->arguments()) current_element(it) = foldTraverse(current_element(it));
		return call;
	  }

	  case EXPCOMPREF:
	  {
		AST_Expression_ComponentReference cr = e->getAsComponentReference();
		if (_pre->find(cr->name()) != _pre->end())
		  return newAST_Expression_Call(_S("pre"), NULL , newAST_SimpleList(static_cast<AST_Expression>(cr)));
		return e;
	  }

	  case EXPBOOLEANNOT:
	  {
		AST_Expression_BooleanNot no = e->getAsBooleanNot();
		return newAST_Expression_BooleanNot(foldTraverse( no->exp())) ;
	  }

	  default:
	  {
		return e;
	  }
  }
};



FindReference::FindReference(AST_String var) {
  _var = var;
}

bool FindReference::foldTraverseElement(bool b1, bool b2, BinOpType t ) {
	return b1 || b2 ;
}


bool FindReference::foldTraverseElementUMinus(AST_Expression u) 
{
	return foldTraverse(u->getAsUMinus()->exp());
}

bool FindReference::foldTraverseElement(AST_Expression e) 
{
  switch (e->expressionType()) {
	  case EXPOUTPUT :
	  {
		AST_Expression_Output b = e->getAsOutput();
		return foldTraverse(b->expressionList()->front()) ;
	  }

	  case EXPCALL:
	  {
		AST_Expression_Call call = e->getAsCall();
		AST_ExpressionListIterator it;
		bool b = false;
		foreach(it , call->arguments()) b |= foldTraverse(current_element(it));
		return b;
	  }

	  case EXPCOMPREF:
	  {
		AST_Expression_ComponentReference cr = e->getAsComponentReference();
		return cr->name() == toStr(_var);
	  }

	  case EXPBOOLEANNOT:
	  {
		AST_Expression_BooleanNot no = e->getAsBooleanNot();
		return foldTraverse( no->exp()) ;
	  }
	  
	  case EXPIF:
	  {
		AST_Expression_If i = e->getAsIf();
		bool eq1 = foldTraverse(i->then());
		bool eq2 = foldTraverse(i->else_exp());
		bool cond = foldTraverse(i->condition());
		return eq1 || eq2 || cond;
	  }

	  default:
	  {
		return false;
	  }
  }
};


ReplaceReference::ReplaceReference(AST_String pre , AST_String post) {
  _pre = pre;
  _post = post;
}

AST_Expression ReplaceReference::foldTraverseElement(AST_Expression b1, AST_Expression b2, BinOpType t ) 
{
	return newAST_Expression_BinOp(b1,b2,t);
}


AST_Expression ReplaceReference::foldTraverseElementUMinus(AST_Expression u) 
{
	return newAST_Expression_UnaryMinus( foldTraverse(u->getAsUMinus()->exp()) );
}

AST_Expression ReplaceReference::foldTraverseElement(AST_Expression e) 
{
  switch (e->expressionType()) {
	  case EXPOUTPUT :
	  {
		AST_Expression_Output b = e->getAsOutput();
		return newAST_Expression_OutputExpressions(newAST_SimpleList(foldTraverse(b->expressionList()->front())));
	  }

	  case EXPCALL:
	  {
		AST_Expression_Call call = e->getAsCall();
		AST_ExpressionListIterator it;
		foreach(it , call->arguments()) current_element(it) = foldTraverse(current_element(it));
		return call;
	  }

	  case EXPCOMPREF:
	  {
		 
		AST_Expression_ComponentReference cr = e->getAsComponentReference();
		if (cr->name() ==  toStr(_pre))
		  return newAST_Expression_ComponentReferenceExp (_post);;
		return e;
	  }

	  case EXPBOOLEANNOT:
	  {
		AST_Expression_BooleanNot no = e->getAsBooleanNot();
		return newAST_Expression_BooleanNot(foldTraverse( no->exp())) ;
	  }

	  default:
	  {
		return e;
	  }
  }
};

