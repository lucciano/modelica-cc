
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

#ifndef AST_BUILDER
#define AST_BUILDER

#include <iostream>
#include <list>
#include <string>

#define DEFINE_TYPE(X) class X##_; typedef X##_ *X;
#define DEFINE_LIST(X) typedef std::list<X>  *X##List; typedef std::list<X>::iterator X##ListIterator; typedef std::list<X>::reverse_iterator X##ListReverseIterator;
#define _S(s) newAST_String(s)
#define MAKE_SPACE for (int __i=0;__i<depth;__i++) ret << " ";
#define BEGIN_BLOCK depth+=2;
#define END_BLOCK depth-=2;
#define GET_AS(X) AST_##X getAs##X ();
#define GET_AS_IMP(X,Y) AST_##X##Y AST_##X##_::getAs##X##Y () { return dynamic_cast<AST_##X##Y > (this); }
extern int depth;

class MCC_Parser;

using namespace std;

/* Basic AST Types */
typedef bool                                 AST_Boolean;
typedef string                              *AST_String;
typedef int                                  AST_Integer;
typedef double                               AST_Real;
typedef string                               AST_Type;
typedef int                                  AST_TypePrefix;
typedef int                                  AST_ClassPrefix;

DEFINE_TYPE(AST_Argument);
DEFINE_TYPE(AST_ArgumentModification);
DEFINE_TYPE(AST_Class);
DEFINE_TYPE(AST_Comment) ;
DEFINE_TYPE(AST_Component) ;
DEFINE_TYPE(AST_Composition);
DEFINE_TYPE(AST_CompositionElement);
DEFINE_TYPE(AST_CompositionEqsAlgs);
DEFINE_TYPE(AST_Declaration);
DEFINE_TYPE(AST_Element);
DEFINE_TYPE(AST_Element_ClassWrapper);
DEFINE_TYPE(AST_Equation);
DEFINE_TYPE(AST_Equation_Call);
DEFINE_TYPE(AST_Equation_Connect);
DEFINE_TYPE(AST_Equation_Else);
DEFINE_TYPE(AST_Equation_Equality);
DEFINE_TYPE(AST_Equation_For);
DEFINE_TYPE(AST_Equation_If);
DEFINE_TYPE(AST_Equation_When);
DEFINE_TYPE(AST_Expression);
DEFINE_TYPE(AST_Expression_BinOp);
DEFINE_TYPE(AST_Expression_Boolean);
DEFINE_TYPE(AST_Expression_BooleanNot);
DEFINE_TYPE(AST_Expression_Brace);
DEFINE_TYPE(AST_Expression_Call);
DEFINE_TYPE(AST_Expression_CallArgs);
DEFINE_TYPE(AST_Expression_Colon);
DEFINE_TYPE(AST_Expression_ComponentReference);
DEFINE_TYPE(AST_Expression_ComponentReference);
DEFINE_TYPE(AST_Expression_Derivative);
DEFINE_TYPE(AST_Expression_End);
DEFINE_TYPE(AST_Expression_If);
DEFINE_TYPE(AST_Expression_If_ElseIf);
DEFINE_TYPE(AST_Expression_Integer);
DEFINE_TYPE(AST_Expression_Null);
DEFINE_TYPE(AST_Expression_Output);
DEFINE_TYPE(AST_Expression_Range);
DEFINE_TYPE(AST_Expression_Real);
DEFINE_TYPE(AST_Expression_String);
DEFINE_TYPE(AST_Expression_UMinus);
DEFINE_TYPE(AST_ExtendsClause);
DEFINE_TYPE(AST_ForIndex);
DEFINE_TYPE(AST_ImportClause);
DEFINE_TYPE(AST_Modification);
DEFINE_TYPE(AST_ModificationAssign);
DEFINE_TYPE(AST_ModificationClass);
DEFINE_TYPE(AST_ModificationEqual);
DEFINE_TYPE(AST_ShortClassExp);
DEFINE_TYPE(AST_Statement);
DEFINE_TYPE(AST_Statement_Break);
DEFINE_TYPE(AST_Statement_Else);
DEFINE_TYPE(AST_Statement_Return);
DEFINE_TYPE(AST_StoredDefinition) ;

/* Lists */
DEFINE_LIST(AST_Argument);
DEFINE_LIST(AST_Class);
DEFINE_LIST(AST_Component);
DEFINE_LIST(AST_CompositionElement);
DEFINE_LIST(AST_Declaration);
DEFINE_LIST(AST_Element);
DEFINE_LIST(AST_Equation);
DEFINE_LIST(AST_Equation_Else);
DEFINE_LIST(AST_Expression);
DEFINE_LIST(AST_ExpressionList);
DEFINE_LIST(AST_Expression_ComponentReference);
DEFINE_LIST(AST_ForIndex);
DEFINE_LIST(AST_Statement);
DEFINE_LIST(AST_Statement_Else);
DEFINE_LIST(AST_String);

/* Enums */
enum BinOpType { BINOPOR, BINOPAND, BINOPLOWER, BINOPLOWEREQ, BINOPGREATER, BINOPGREATEREQ, BINOPCOMPNE, BINOPCOMPEQ,
                 BINOPADD, BINOPELADD, BINOPSUB, BINOPELSUB, BINOPDIV, BINOPELDIV, BINOPMULT, BINOPELMULT, BINOPEXP, BINOPELEXP };
enum ElementType { ELNONE, COMPONENT, IMPORT, EXTENDS, ELCLASS };
enum EquationType { EQNONE, EQEQUALITY, EQCONNECT,EQCALL, EQFOR, EQWHEN, EQIF };
enum ExpressionType { EXPNONE ,EXPCOMPREF, EXPBINOP , EXPDERIVATIVE , EXPNULL, EXPEND, EXPIF, EXPCALLARG, EXPBRACE,
                      EXPCALL, EXPELSEIF, EXPCOLON, EXPUMINUS, EXPBOOLEAN, EXPSTRING, EXPREAL, EXPINTEGER, EXPBOOLEANNOT, EXPOUTPUT, EXPRANGE };
enum ModificationType { MODNONE ,MODEQUAL, MODASSIGN, MODCLASS };

/* Constructors */

/* Argument modification */
AST_ArgumentList newAST_ArgumentList();
AST_Argument newAST_ArgumentModification(AST_String,AST_Modification);
AST_Argument newAST_Redeclaration(AST_Boolean, AST_Boolean, AST_Argument); 
AST_Argument newAST_ShortClass(AST_ClassPrefix,AST_String, AST_ShortClassExp);
AST_ShortClassExp newAST_ShortClassExp(AST_TypePrefix, AST_String, AST_ExpressionList, AST_ArgumentList);
AST_ShortClassExp newAST_ShortClassExp_Enum(AST_StringList); 
AST_Argument AST_ArgumentSet(AST_Boolean, AST_Boolean, AST_Argument);
AST_Argument newAST_ElementModification(AST_String, AST_Modification);
AST_Argument AST_ArgumentSetReplaceable(AST_Argument);
AST_Comment newAST_Comment(AST_String, AST_ArgumentList);

/* Strings */
AST_String newAST_String(AST_String);
AST_String newAST_StringNull();
AST_String newAST_String(string s);
AST_String newAST_String(char * s);
AST_String newAST_String(const char * s);
AST_String newAST_String(AST_String);
AST_String newAST_DotString(AST_String);
AST_String AST_StringDotAppend(AST_String,AST_String);
AST_StringList newAST_StringList();
AST_String copyAST_String(AST_String);
#define toStr(it) (*it)

/* Classes */
AST_Class newAST_Class(AST_String, AST_Composition);
AST_Class newAST_Class(AST_String);
AST_Class newAST_ClassEnumeration(AST_String, AST_StringList);
AST_Class newAST_ClassDerivation(AST_String, AST_String, AST_StringList);
AST_Class newAST_ClassExtends(AST_String, AST_Composition);
AST_Class newAST_ClassModification(AST_String, AST_TypePrefix, AST_String, AST_ExpressionList,AST_ArgumentList);
AST_ClassList newAST_ClassList();
AST_Class newAST_Class(AST_String s);
AST_Class AST_Class_SetFinal(AST_Class,AST_Boolean);
AST_Class AST_Class_SetPrefixEncapsulated(AST_Class,AST_ClassPrefix,AST_Boolean);

/* Classes Prefixes */
AST_ClassPrefix AST_ClassPrefix_AddPartial(AST_Boolean, AST_ClassPrefix);
AST_ClassPrefix AST_ClassPrefix_Class();
AST_ClassPrefix AST_ClassPrefix_Model();
AST_ClassPrefix AST_ClassPrefix_Record(AST_Boolean);
AST_ClassPrefix AST_ClassPrefix_Block();
AST_ClassPrefix AST_ClassPrefix_Connector(AST_Boolean);
AST_ClassPrefix AST_ClassPrefix_Type();
AST_ClassPrefix AST_ClassPrefix_Package();
AST_ClassPrefix AST_ClassPrefix_Function(AST_ClassPrefix);
AST_ClassPrefix AST_ClassPrefix_Operator();
AST_ClassPrefix AST_ClassPrefix_None();
AST_ClassPrefix AST_ClassPrefix_Pure(AST_Boolean);
AST_ClassPrefix AST_ClassPrefix_Impure(AST_Boolean);

/* Compositions */
AST_Composition newAST_Composition();
AST_Composition newAST_Composition(AST_ElementList );
AST_Composition newAST_Composition(AST_ElementList el, AST_CompositionElementList cl);
AST_Composition newAST_Composition(AST_CompositionElementList cl);
AST_CompositionEqsAlgs newAST_NullCompositionEquations();
AST_CompositionEqsAlgs newAST_CompositionInitialEquations(AST_EquationList);
AST_CompositionEqsAlgs newAST_CompositionEquations(AST_EquationList);
AST_CompositionEqsAlgs newAST_CompositionInitialAlgorithms(AST_StatementList);
AST_CompositionEqsAlgs newAST_CompositionAlgorithms(AST_StatementList);

/* Components and elements */
AST_Component newAST_Component(AST_DeclarationList cl, AST_String type, AST_TypePrefix, AST_ExpressionList );
AST_ComponentList newAST_ComponentList(AST_DeclarationList);
AST_ComponentList newAST_ComponentList();
AST_ElementList newAST_ElementList();
AST_ImportClause newAST_ImportClause(AST_String);
AST_Element AST_ImportToElement(AST_ImportClause i);
AST_Element AST_ComponentToElement(AST_Component c);
AST_Element AST_ExtendsToElement(AST_ExtendsClause e);
AST_ExtendsClause newAST_ExtendsClause(AST_String);
AST_Element newAST_Element_ClassWrapper(AST_Class);
AST_CompositionElementList newAST_CompositionElementList();
AST_Declaration newAST_Declaration(AST_String,AST_ExpressionList, AST_Modification);
AST_Declaration AST_Declaration_AddCondComment(AST_Declaration, AST_Expression, AST_Comment);
AST_DeclarationList newAST_DeclarationList(AST_Declaration);

AST_CompositionElement newAST_CompositionElement(AST_CompositionEqsAlgs);
AST_CompositionElement newAST_CompositionElement(AST_ElementList);

/* Expressions */
AST_ExpressionList newAST_ExpressionList(AST_Expression);
AST_ExpressionList newAST_ExpressionList();
AST_ExpressionListList newAST_ExpressionListList();
AST_Expression newAST_Expression_Integer(AST_Integer i);
AST_Expression newAST_Expression_Real(AST_Real r);
AST_Expression newAST_Expression_String(AST_String s);
AST_Expression newAST_Expression_Boolean(AST_Boolean b);
AST_Expression newAST_Expression_Derivative(AST_ExpressionList el);
AST_Expression_ComponentReference newAST_Expression_ComponentReference();
AST_Expression_ComponentReference AST_Expression_ComponentReference_Add(AST_Expression_ComponentReference, AST_String, AST_ExpressionList );
AST_Expression_ComponentReference AST_Expression_ComponentReference_AddDot(AST_Expression_ComponentReference, AST_String, AST_ExpressionList);
AST_Expression newAST_Expression_ComponentReferenceExp(AST_String);
AST_Expression newAST_Expression_ComponentReferenceExp(AST_String s, AST_String s2);
AST_Expression newAST_Expression_ComponentReferenceExpDot(AST_String s, AST_String s2);
AST_Expression AST_Expression_ComponentReferenceExpAddDot(AST_Expression_ComponentReference, AST_String, AST_String, AST_ExpressionList);
AST_Expression AST_Expression_ComponentReferenceExpAdd(AST_Expression_ComponentReference, AST_String, AST_String, AST_ExpressionList);
AST_Expression newAST_Expression_BinOp(AST_Expression, AST_Expression, BinOpType);
AST_Expression newAST_Expression_UnaryMinus(AST_Expression);
AST_Expression newAST_Expression_If(AST_Expression, AST_Expression, AST_ExpressionList, AST_Expression);
AST_Expression newAST_Expression_Null();
AST_Expression newAST_Expression_Colon();
AST_Expression newAST_Expression_End();
AST_Expression newAST_Expression_Call(AST_String,AST_String,AST_ExpressionList);
AST_Expression newAST_Expression_DotCall(AST_String,AST_String,AST_ExpressionList);
AST_Expression newAST_Expression_BooleanNot(AST_Expression);
AST_Expression newAST_Expression_ElseIf(AST_Expression,AST_Expression);
AST_Expression newAST_Expression_NamedArgument(AST_String, AST_Expression);
AST_Expression newAST_Expression_FunctionCallArgs(AST_ExpressionList);
AST_Expression newAST_Expression_Function(AST_String, AST_ExpressionList);
AST_Expression newAST_Expression_OutputExpressions(AST_ExpressionList);
AST_Expression newAST_Expression_Range(AST_Expression, AST_Expression);
AST_Expression newAST_Expression_Brace(AST_ExpressionList);
AST_Expression newAST_BracketExpList(AST_ExpressionListList);

/* Equations */
AST_EquationList newAST_EquationList();
AST_Equation newAST_Equation_Equality(AST_Expression, AST_Expression);
AST_Equation newAST_Equation_Connect(AST_Expression_ComponentReference, AST_Expression_ComponentReference);
AST_Equation newAST_Equation_If(AST_Expression e, AST_EquationList eql, AST_Equation_ElseList, AST_EquationList);
AST_Equation newAST_Equation_For(AST_ForIndexList, AST_EquationList eql);
AST_Equation newAST_Equation_When(AST_Expression,AST_EquationList, AST_Equation_ElseList);
AST_Equation newAST_Equation_Call(AST_Expression);
AST_ForIndex newAST_ForIndex(AST_String, AST_Expression);
AST_ForIndexList newAST_ForIndexList();
AST_Equation_ElseList newAST_Equation_ElseList();
AST_Equation_Else newAST_Equation_Else(AST_Expression, AST_EquationList);

/* Modification */
AST_Modification newAST_ModificationClass(AST_ArgumentList,AST_Expression); 
AST_Modification newAST_ModificationEqual(AST_Expression);
AST_Modification newAST_ModificationAssign(AST_Expression); 
AST_Modification newAST_ModificationNull();

/* Statements */
AST_Statement newAST_Statement_Break();
AST_Statement newAST_Statement_Return();
AST_Statement newAST_Statement_While(AST_Expression, AST_StatementList);
AST_Statement newAST_Statement_When(AST_Expression,AST_StatementList, AST_Statement_ElseList);
AST_Statement newAST_Statement_For(AST_ForIndexList , AST_StatementList);
AST_Statement newAST_Statement_If(AST_Expression , AST_StatementList, AST_Statement_ElseList, AST_StatementList);
AST_Statement newAST_Statement_Assign(AST_Expression_ComponentReference, AST_Expression);
AST_Statement newAST_Statement_OutputAssign(AST_ExpressionList, AST_Expression_ComponentReference, AST_ExpressionList);
AST_StatementList  newAST_StatementList();
AST_Statement_Else newAST_Statement_Else(AST_Expression, AST_StatementList);
AST_Statement_ElseList newAST_Statement_ElseList();

/* Stored Defintion */
AST_StoredDefinition newAST_StoredDefinition(AST_ClassList,AST_String);

/* Type prefixes*/ 
enum TypePrefix {
  TP_INPUT      =(1<<1),
  TP_OUTPUT     =(1<<2),
  TP_DISCRETE   =(1<<3),
  TP_PARAMETER  =(1<<4),
  TP_FLOW       =(1<<5),
  TP_STREAM     =(1<<6),
  TP_CONSTANT   =(1<<7)
};
AST_TypePrefix newAST_TypePrefix();
AST_TypePrefix newAST_TypePrefix(AST_TypePrefix);
AST_TypePrefix AST_TypePrefixAdd(AST_TypePrefix,AST_TypePrefix);

/* Class prefixes */
enum ClassPrefix {
  CP_PARTIAL    = (1<<1),
  CP_CLASS      = (1<<2),
  CP_MODEL      = (1<<3),
  CP_BLOCK      = (1<<4),
  CP_RECORD     = (1<<5),
  CP_CONNECTOR  = (1<<6),
  CP_TYPE       = (1<<7),
  CP_OPERATOR   = (1<<8),
  CP_EXPANDABLE = (1<<8),
  CP_PURE       = (1<<10),
  CP_IMPURE     = (1<<11),
  CP_PACKAGE    = (1<<12),
  CP_FUNCTION   = (1<<13)
};

/* Printers */
#define DEFINE_PRINTER(X) ostream & operator<<(ostream &os , const X &x ) ;
DEFINE_PRINTER(AST_Argument);
DEFINE_PRINTER(AST_Comment);
DEFINE_PRINTER(AST_CompositionElement);
DEFINE_PRINTER(AST_Composition);
DEFINE_PRINTER(AST_Class);
DEFINE_PRINTER(AST_Expression);
DEFINE_PRINTER(AST_Expression_ComponentReference);
DEFINE_PRINTER(AST_Equation);
DEFINE_PRINTER(AST_Modification);
DEFINE_PRINTER(AST_Statement);
DEFINE_PRINTER(AST_StoredDefinition);
DEFINE_PRINTER(AST_String);

/* List utils */

#define current(it) (*it)
#define foreach(it,list) for (it=list->begin();it!=list->end();it++)
#define foreachReverse(it,list) for (it=list->rbegin();it!=list->rend();it++)


template <typename T1>
list<T1> * AST_ListAppend(list<T1> *l, T1 e) { l->push_back(e); return l; }

template <typename T1>
list<T1> * AST_ListPrepend(list<T1> *l, T1 e) { l->push_front(e); return l; }

template <typename T1>
list<T1> * AST_ListRemove(list<T1> *l, T1 e) { l->remove(e); return l; }

template <typename T1>
T1 AST_ListFirst(list<T1> *l) {return l->front();}


template <typename T1>
list<T1> * AST_ListConcat(list<T1> *l1,list<T1> *l2) {
	l1->insert(l1->end(), l2->begin() , l2->end());
	return l1;
}

template <typename T1>
list<T1> * newAST_SimpleList(T1  t) {
	list<T1> * ls = new list<T1>;
	ls->push_back(t);
	return ls;
}

template <typename T1>
void AST_ListPrint(list<T1> *l1, ostream &ret, string sec_name="", string separator=" ", string opener="",string closer="", bool block=false) {
  typename list<T1>::iterator it;
  int size=l1->size(),i=1;
  if (size) {
    ret << sec_name;
    if (block) BEGIN_BLOCK;
    ret << opener;
    foreach(it,l1)  {
      ret << current(it);
      ret << (i<size ? separator : "");
      i++;
    }
    ret << closer;
    if (block) END_BLOCK;
  }
}


#endif
