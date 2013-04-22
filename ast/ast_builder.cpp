
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
#include <ast/stored_definition.h>
#include <ast/class.h>
#include <ast/composition.h>
#include <ast/expression.h>
#include <ast/equation.h>
#include <ast/statement.h>
#include <cassert>

class MCC_Parser;
extern int depth=0;
MCC_Parser *parser = NULL;
AST_StoredDefinition root;

AST_StoredDefinition newAST_StoredDefinition(AST_ClassList cl, AST_String within) { 
  return new AST_StoredDefinition_(cl,within);
}

AST_String newAST_String(string s) {
  return new string(s);
}

AST_String newAST_String(AST_String s) {
  AST_String ret = new string(*s);
  delete s;
  return ret;
}

AST_String newAST_DotString(AST_String s) {
  AST_String ret = new string(*s);
  ret->insert(0,".");
  delete s;
  return ret;
}

AST_String AST_StringDotAppend(AST_String ret, AST_String a) {
  if (ret==NULL) 
    ret = new string();
  if (a!=NULL) {
    ret->append(".");
    ret->append(*a);
    delete a;
  }
  return ret;
}

AST_String newAST_StringNull() { 
  return NULL; 
}


AST_ClassList newAST_ClassList() {
  return new list<AST_Class>();
}

AST_Composition newAST_Composition() {
  return new AST_Composition_(newAST_ElementList(),newAST_CompositionElementList());
}

AST_Composition newAST_Composition(AST_ElementList el) {
  return new AST_Composition_(el,newAST_CompositionElementList());
}

AST_Composition newAST_Composition(AST_ElementList el, AST_CompositionElementList cl) {
  return new AST_Composition_(el,cl);
}

AST_Composition newAST_Composition(AST_CompositionElementList cl)
{
  return new AST_Composition_(newAST_ElementList(),cl);
}

AST_ElementList newAST_ElementList() {
  return new list<AST_Element>();
}

AST_Element AST_ImportToElement(AST_ImportClause i) { 
  return (AST_Element)i; 
}

AST_Element AST_ExtendsToElement(AST_ExtendsClause e) {
  return (AST_Element)e; 
}

AST_ExtendsClause newAST_ExtendsClause(AST_String name) {
  AST_ExtendsClause ret = new AST_ExtendsClause_(*name);
  delete name;
  return ret;
}

AST_Element newAST_Element_ClassWrapper(AST_Class c) {
  return new AST_Element_ClassWrapper_(c);
}

AST_Element AST_ComponentToElement(AST_Component c) {
  return (AST_Element)c;
}

AST_CompositionElementList newAST_CompositionElementList() {
  return new list<AST_CompositionElement>();
}

AST_ImportClause newAST_ImportClause(AST_String) {
  return new AST_ImportClause_("dummy");
}

AST_Expression newAST_Expression_Integer(int i) {
  return new AST_Expression_Integer_(i);
}

AST_ExpressionList newAST_ExpressionList(AST_Expression e) {
  AST_ExpressionList l=new list<AST_Expression>();
  l->push_back(e);
  return l;
}

AST_ExpressionList newAST_ExpressionList() {
  AST_ExpressionList l=new list<AST_Expression>();
  return l;
}

AST_Argument AST_ArgumentSet(bool b1, bool b2, AST_Argument arg) {
  return arg;
}

AST_EquationList newAST_EquationList() {
  return new list<AST_Equation>();
}

AST_Equation newAST_Equation_Equality(AST_Expression left, AST_Expression right) {
  return new AST_Equation_Equality_(left,right);
}

AST_Equation newAST_Equation_Connect(AST_Expression_ComponentReference cr1, AST_Expression_ComponentReference cr2) {
  return new AST_Equation_Connect_(cr1,cr2);
}

AST_Expression newAST_Expression_Real(AST_Real r){ 
  return new AST_Expression_Real_(r);
}

AST_Expression newAST_Expression_String(AST_String s) {
  AST_Expression_String ret=new AST_Expression_String_(*s);
  delete s;
  return ret;
}

AST_Expression newAST_Expression_Boolean(AST_Boolean b) {
  return new AST_Expression_Boolean_(b);
}

AST_Expression newAST_Expression_Derivative(AST_ExpressionList el) {
  return new AST_Expression_Derivative_(el);
}

AST_Class newAST_Class(AST_String s) { 
  return newAST_Class(s,newAST_Composition()); 
}

AST_Class newAST_Class(AST_String s, AST_Composition comp) { 
  return new AST_Class_(s,comp);
}

AST_Component newAST_Component(AST_DeclarationList cl, AST_String type, AST_TypePrefix tp) {
  AST_Component c= new AST_Component_(cl,*type,tp);
  delete type;
  return c;
}

AST_Declaration newAST_Declaration(AST_String s) {
  AST_Declaration ret = new AST_Declaration_(*s);
  delete s;
  return ret;
}

AST_DeclarationList newAST_DeclarationList(AST_Declaration d) {
  AST_DeclarationList dl = new list<AST_Declaration>();
  dl->push_back(d);
  return dl;
}

AST_CompositionElement newAST_CompositionElement(AST_EquationList el) {
  //return new AST_CompositionElement_(el);
}

AST_CompositionElement newAST_CompositionElement(AST_ElementList el) {
  return new AST_CompositionElement_(el);
}

AST_Expression newAST_Expression_ComponentReferenceExp(AST_String s) {
  return newAST_Expression_ComponentReference(s);
}

AST_Expression newAST_Expression_ComponentReferenceExp(AST_String s, AST_String s2) {
  if (s2!=NULL) {
    s->append(*s2);
    delete s2;
  }
  return newAST_Expression_ComponentReference(s);
}

AST_Expression newAST_Expression_ComponentReferenceExpDot(AST_String s, AST_String rest) {
  s->insert(0,".");
  s->append(rest->c_str());
  delete rest;
  return newAST_Expression_ComponentReference(s);
}

AST_Expression_ComponentReference newAST_Expression_ComponentReference() {
  return new AST_Expression_ComponentReference_("");
}

AST_Expression_ComponentReference AST_Expression_ComponentReference_AddDot(AST_Expression_ComponentReference cr, AST_String s, AST_ExpressionList subs) {
  string n = cr->name();
  n.insert(0,".");
  n.insert(0,*s);
  n.insert(0,".");
  cr->setName(n);
  delete s;
  return cr;
}

AST_Expression_ComponentReference AST_Expression_ComponentReference_Add(AST_Expression_ComponentReference cr, AST_String s, AST_ExpressionList subs) {
  string n = cr->name();
  if  (n!= "") 
    n.insert(0,".");
  n.insert(0,*s);
  cr->setName(n);
  delete s;
  return cr;
}

AST_Expression_ComponentReference AST_Expression_ComponentReference_Add2(AST_Expression_ComponentReference cr, AST_String s) {
  string n = cr->name();
  n.insert(0,*s);
  cr->setName(n);
  delete s;
  return cr;
}

AST_Expression_ComponentReference newAST_Expression_ComponentReference(AST_String s) {
  AST_Expression_ComponentReference cr = new AST_Expression_ComponentReference_(*s);
  delete s;
  return cr;
}

ostream & operator<<(ostream &os , const AST_CompositionElement &ce) {
  assert(ce!=NULL);
  os << *ce;
  return os;
}

ostream & operator<<(ostream &os , const AST_String &s) {
  os << *s;
  return os;
}

AST_ComponentList newAST_ComponentList() {
  return new list<AST_Component>(); 
}

AST_TypePrefix newAST_TypePrefix() {
  return 0;
}

AST_TypePrefix newAST_TypePrefix(AST_TypePrefix tp) {
  return tp;
}

AST_TypePrefix AST_TypePrefixAdd(AST_TypePrefix tp, AST_TypePrefix tpadd) {
  return tp | tpadd;
}

AST_Expression newAST_Expression_BinOp(AST_Expression left, AST_Expression right, BinOpType type) {
  return new AST_Expression_BinOp_(left,right,type);
}

AST_Expression newAST_Expression_UnaryMinus(AST_Expression e) {
  return new AST_Expression_UMinus_(e);
}

AST_Expression newAST_Expression_If(AST_Expression cond, AST_Expression then, AST_ExpressionList else_list, AST_Expression else_exp) {
  return new AST_Expression_If_(cond,then,else_exp,else_list);
};

AST_Equation newAST_Equation_If(AST_Expression e, AST_EquationList eql, AST_Equation_ElseList elseif, AST_EquationList elseeqs) {
  return new AST_Equation_If_(e,eql);
}

AST_StringList newAST_StringList() {
  return new list<AST_String>();
}

AST_Expression newAST_Expression_End() {
  return new AST_Expression_End_();
} 

AST_Expression newAST_Expression_Null() {
  return new AST_Expression_Null_();
} 

AST_Expression newAST_Expression_DotCall(AST_String name, AST_String rest) {
  name->insert(0,"."); 
  if (rest!=NULL) {
    name->append(rest->c_str());
    delete rest;
  }
  return new AST_Expression_Call_(name);
}

AST_Expression newAST_Expression_Call(AST_String name, AST_String rest) {
  if (rest!=NULL) {
    name->append(rest->c_str());
    delete rest;
  }
  return new AST_Expression_Call_(name);
}

AST_Expression newAST_Expression_Colon() {
  return new AST_Expression_Colon_();
}

AST_Expression newAST_Expression_BooleanNot(AST_Expression e) {
  return new AST_Expression_BooleanNot_(e);
}

AST_Equation newAST_Equation_Call(AST_Expression e) {
  if (e->expressionType()!=EXPCALL) { 
    cerr << "Not valid equation" << endl;
    exit(-1);
  }
  return new AST_Equation_Call_(e);
}

AST_ClassPrefix AST_ClassPrefix_Class() {
  return CP_CLASS;
}

AST_ClassPrefix AST_ClassPrefix_Model() { 
  return CP_MODEL;
}

AST_ClassPrefix AST_ClassPrefix_Record(AST_Boolean oper) {
  if (oper)
    return CP_RECORD | CP_OPERATOR;
  return CP_RECORD;
}

AST_ClassPrefix AST_ClassPrefix_Block() {
  return CP_BLOCK;
}

AST_ClassPrefix AST_ClassPrefix_Connector(AST_Boolean expandable) {
  if (expandable)
    return CP_CONNECTOR | CP_EXPANDABLE;
  return CP_CONNECTOR;
}

AST_ClassPrefix AST_ClassPrefix_Type() {
  return CP_TYPE;
}

AST_ClassPrefix AST_ClassPrefix_Package() {
  return CP_PACKAGE;
}

AST_ClassPrefix AST_ClassPrefix_Function(AST_ClassPrefix p) {
  return CP_FUNCTION | p;
}

AST_ClassPrefix AST_ClassPrefix_Operator() {
  return CP_OPERATOR;
}

AST_ClassPrefix AST_ClassPrefix_Impure(AST_Boolean oper) {
  if (oper)
    return CP_IMPURE | CP_OPERATOR;
  return CP_IMPURE;
}

AST_ClassPrefix AST_ClassPrefix_Pure(AST_Boolean oper) {
  if (oper)
    return CP_PURE | CP_OPERATOR;
  return CP_PURE;
}

AST_ClassPrefix AST_ClassPrefix_None() {
  return 0;
}

AST_ClassPrefix AST_ClassPrefix_AddPartial(AST_Boolean partial, AST_ClassPrefix p) {
  if (partial)
    return p | CP_PARTIAL;
  return p;
}

AST_Equation newAST_EquationFor(AST_ForIndexList ind, AST_EquationList eql) {
  return new AST_Equation_For_(ind,eql);
}

AST_Expression newAST_Expression_Else(AST_Expression,AST_Expression) {
}

AST_ForIndex newAST_ForIndex(AST_String ident, AST_Expression exp) {
  return new AST_ForIndex_(ident,exp);
}

AST_ForIndexList newAST_ForIndexList() {
  return new list<AST_ForIndex>();
}

AST_Equation newAST_Equation_For(AST_ForIndexList ind, AST_EquationList eql) {
  return new AST_Equation_For_(ind,eql);
}

AST_Class AST_Class_SetFinal(AST_Class c,AST_Boolean final) {
  if (final)  
    c->setFinal();
  return c;
}

AST_Class AST_Class_SetPrefixEncapsulated(AST_Class c, AST_ClassPrefix prefix, AST_Boolean enc) {
  if (enc)
    c->setEncapsulated();
  c->setPrefixes(prefix);
  return c;
}

AST_Expression AST_Expression_ComponentReferenceExpAddDot(AST_Expression_ComponentReference cr, AST_String id, AST_String rest , AST_ExpressionList subs) {
  if (rest!=NULL) {
    id->append(rest->c_str());
    delete rest;
  }
  return AST_Expression_ComponentReference_AddDot(cr,id,subs);
}

AST_Expression AST_Expression_ComponentReferenceExpAdd(AST_Expression_ComponentReference cr, AST_String id, AST_String rest , AST_ExpressionList subs) {
  if (rest!=NULL) {
    id->append(rest->c_str());
    delete rest;
  }
  return AST_Expression_ComponentReference_Add(cr,id,subs);
}

AST_Expression newAST_Expression_NamedArgument(AST_String, AST_Expression) {
}

AST_Expression newAST_Expression_FunctionCallArgs(AST_ExpressionList) {
}


AST_Expression newAST_Expression_Function(AST_String, AST_ExpressionList) {
}

AST_Equation_ElseList newAST_Equation_ElseIfList() {
  return new list<AST_Equation_Else>();
}

AST_Equation_Else newAST_Equation_Else(AST_Expression cond, AST_EquationList eqs) {
  return new AST_Equation_Else_(cond,eqs);
}

AST_StatementList  newAST_StatementList() { 
  return new list<AST_Statement>();
}

AST_Statement newAST_Statement_Break() {
  return new AST_Statement_Break_();
}
AST_Statement newAST_Statement_Return() {
  return new AST_Statement_Return_();

}
AST_Modification newAST_ModificationNull() {
  return NULL;
}

AST_Statement_ElseList newAST_Statement_ElseList() {
  return new list<AST_Statement_Else>();
}

AST_Equation_ElseList newAST_Equation_ElseList() {
  return new list<AST_Equation_Else>();
}

AST_ArgumentList newAST_ArgumentList() {
  return new list<AST_Argument>();
}

AST_ExpressionListList newAST_ExpressionListList() {
  return new list<AST_ExpressionList>();
}

AST_Argument AST_ArgumentSetReplaceable(AST_Argument arg) {
  return arg;
}

AST_Modification newAST_ModificationEqual(AST_Expression) {
  return NULL;
}
AST_Modification newAST_ModificationAssign(AST_Expression) {
  return NULL;
}

AST_Expression newAST_Expression_Range(AST_Expression, AST_Expression) {
  return newAST_Expression_Null();
}

AST_Expression newAST_Expression_OutputExpressions(AST_ExpressionList exp_list) { 
  /* For now just one element supported */
  return new AST_Expression_Output_(exp_list);
}

AST_Expression newAST_Expression_FunctionArguments(AST_ExpressionList) {
  return newAST_Expression_Null();
}

AST_Expression newAST_BracketExpList(AST_ExpressionListList) {
  return newAST_Expression_Null();
}

AST_Expression newAST_Expression_ElseIf(AST_Expression,AST_Expression) {
  return newAST_Expression_Null();
}

AST_Statement_Else newAST_Statement_Else(AST_Expression, AST_StatementList) {
  return NULL;
}

AST_Statement newAST_Statement_If(AST_Expression , AST_StatementList, AST_Statement_ElseList, AST_StatementList) {
  return NULL;
}

AST_Statement newAST_Statement_For(AST_ForIndexList , AST_StatementList) {
  return NULL;
}

AST_Statement newAST_Statement_While(AST_Expression, AST_StatementList) {
  return NULL;
}

AST_Statement newAST_Statement_When(AST_Expression cond, AST_StatementList sts, AST_Statement_ElseList else_st) {
  return new AST_Statement_When_(cond,sts);
}

AST_Statement newAST_Statement_OutputAssign(AST_ExpressionList, AST_Expression_ComponentReference, AST_ExpressionList) {
  return NULL;
}
AST_Statement newAST_Statement_Assign(AST_Expression_ComponentReference cr, AST_Expression exp) {
  return new AST_Statement_Assign_(cr,exp);
}

AST_Equation newAST_Equation_When(AST_Expression,AST_EquationList, AST_Equation_ElseList) {
  return NULL;
}

AST_CompositionEqsAlgs newAST_CompositionInitialEquations(AST_EquationList eqlist) {
  return new AST_CompositionEqsAlgs_(eqlist,true);
}

AST_CompositionEqsAlgs newAST_CompositionEquations(AST_EquationList eqlist) {
  return new AST_CompositionEqsAlgs_(eqlist);
}

AST_CompositionEqsAlgs newAST_CompositionInitialAlgorithms(AST_StatementList stlist) {
  return new AST_CompositionEqsAlgs_(stlist,true);
}

AST_CompositionEqsAlgs newAST_CompositionAlgorithms(AST_StatementList stlist) {
  return new AST_CompositionEqsAlgs_(stlist);
}

AST_ShortClassExp newAST_ShortClassExp(AST_TypePrefix, AST_String, AST_ExpressionList, AST_ArgumentList) {
  return NULL;
}
AST_ShortClassExp newAST_ShortClassExp_Enum(AST_StringList) {
  return NULL;
}

AST_Argument newAST_Redeclaration(AST_Boolean, AST_Boolean, AST_Argument) {
  return NULL;
}

AST_Argument newAST_ShortClass(AST_ClassPrefix,AST_String, AST_ShortClassExp) {
  return NULL;
}

AST_Argument newAST_ElementModification(AST_String, AST_Modification) {
  return NULL;
}

AST_Modification newAST_Modification(AST_ArgumentList,AST_Expression) {
  return NULL;
}

AST_CompositionElement newAST_CompositionElement(AST_CompositionEqsAlgs comp_eq_algs) {
  return new AST_CompositionElement_(comp_eq_algs);
}

AST_Class newAST_ClassExtends(AST_String, AST_Composition) {
  return NULL;
}

AST_Class newAST_ClassEnumeration(AST_String, AST_StringList) {
  return NULL;
}

AST_Class newAST_ClassDerivation(AST_String, AST_String, AST_StringList) {
  return NULL;
}

AST_Class newAST_ClassModification(AST_String, AST_TypePrefix, AST_String, AST_ExpressionList,AST_ArgumentList) {
  return NULL;
}

AST_CompositionEqsAlgs newAST_NullCompositionEquations() {
  return NULL;
}
