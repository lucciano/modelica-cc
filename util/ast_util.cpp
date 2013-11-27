
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
#include <util/debug.h>
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

EqualExp::EqualExp(VarSymbolTable symbolTable) {
  _symbolTable = symbolTable;
}

bool EqualExp::equalTraverse(AST_Expression a, AST_Expression b) {
  if (a->expressionType()!=b->expressionType()) return false;
  switch (a->expressionType()) {
  case EXPBINOP:
  {
    AST_Expression_BinOp binOpA = a->getAsBinOp();
    AST_Expression_BinOp binOpB = b->getAsBinOp();
    return equalTraverse(binOpA->left(), binOpB->left()) && equalTraverse(binOpA->right(), binOpB->right());
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
    AST_Expression_ComponentReference compRefA = a->getAsComponentReference();
    AST_Expression_ComponentReference compRefB = b->getAsComponentReference();
    VarInfo varInfoA = getVarInfo(compRefA);
    VarInfo varInfoB = getVarInfo(compRefB);
    TypesType typeA = varInfoA->type()->getType();
    TypesType typeB = varInfoB->type()->getType();
    ERROR_UNLESS(typeA == typeB,
        "EqualExp::equalTraverseElement:\n"
        "AST_Expression_ComponentReference with the same name but different types.\n");
    if (typeA == TYARRAY) {
      return compareArrays(compRefA, compRefB);
    } else {
      return (CREF_NAME(a).compare(CREF_NAME(b)) == 0);
    }
  }
  case EXPDERIVATIVE:
  {
    AST_ExpressionList argsA = a->getAsDerivative()->arguments();
    AST_ExpressionList argsB = b->getAsDerivative()->arguments();
    if (argsA->size() != argsB->size()) return false;
    ERROR_UNLESS(argsA->size() == 1, "EqualExp::equalTraverseElement:\n"
        "AST_Expression_Derivative with more than 1 argument are not supported yet.\n");
    AST_Expression_ComponentReference compRef1 = a->getAsDerivative()->arguments()->front()->getAsComponentReference();
    AST_Expression_ComponentReference compRef2 = b->getAsDerivative()->arguments()->front()->getAsComponentReference();
    return equalTraverse(compRef1, compRef2);
  }
  case EXPREAL:
  {
    AST_Expression_Real realA = a->getAsReal();
    AST_Expression_Real realB = b->getAsReal();
    return realA->val() == realB->val();
  }
  case EXPINTEGER:
  {
    AST_Expression_Integer intA = a->getAsInteger();
    AST_Expression_Integer intB = b->getAsInteger();
    return intA->val() == intB->val();
  }
  default:
    ERROR("EqualExp::equalTraverseElement:\n"
        "Incorrect AST_Expression_Type %s", a->expressionType());
  }
}

VarInfo EqualExp::getVarInfo(AST_Expression_ComponentReference compRef) {
  VarInfo varInfo;
  AST_StringList names = compRef->names();
  if (names->size() > 0) {
    ERROR_UNLESS(names->size() == 1, "EqualExp::getVarInfo\n"
                "AST_Component_Reference with names list bigger than 1 are not supported yet.\n");
    AST_String name = names->front();
    varInfo = _symbolTable->lookup(*name);
  } else {
    varInfo = _symbolTable->lookup(compRef->name());
  }
  return varInfo;
}

bool EqualExp::compareArrays(AST_Expression_ComponentReference arrayA, AST_Expression_ComponentReference arrayB) {
  AST_ExpressionListList indexesListA = arrayA->indexes();
  AST_ExpressionListList indexesListB = arrayB->indexes();
  ERROR_UNLESS(indexesListA->size() == indexesListB->size(), "EqualExp::compareArrays:\n"
      "indexes list sizes should be equal.\n");
  ERROR_UNLESS(indexesListA->size() == 1, "EqualExp::compareArrays:\n"
        "Indexes list sizes greater than 1 are not supported yet.\n");
  AST_ExpressionList indexesA = indexesListA->front();
  AST_ExpressionList indexesB = indexesListA->front();
  ERROR_UNLESS(indexesA->size() == indexesB->size(), "EqualExp::compareArrays:\n"
        "indexes sizes should be equal.\n");
  ERROR_UNLESS(indexesA->size() == 1, "EqualExp::compareArrays:\n"
          "Multidimensional arrays are not supported yet.\n");
  return equalTraverse(indexesA->front(), indexesB->front());
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



AST_Expression ReplaceExp::replaceExp(AST_Expression rep, AST_Expression for_exp, AST_Expression in, VarSymbolTable symbol_table) {
  _rep=rep;
  _for_exp=for_exp;
  _in=in;
  _symbol_table = symbol_table;
  return mapTraverse(in);
}
AST_Expression ReplaceExp::mapTraverseElement(AST_Expression e) {
  EqualExp *equalExp = new EqualExp(_symbol_table);
  if (equalExp->equalTraverse(e,_rep))
    return _for_exp;
  return e;
}

ReplaceBoolean::ReplaceBoolean() {}

AST_Expression ReplaceBoolean::foldTraverseElement(AST_Expression b1, AST_Expression b2, BinOpType t ) {
  return newAST_Expression_BinOp(b1,b2,t);
}

AST_Expression ReplaceBoolean::foldTraverseElementUMinus(AST_Expression u) {
  return newAST_Expression_UnaryMinus(foldTraverse(u->getAsUMinus()->exp()));
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

