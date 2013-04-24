
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

class MCC_Parser;
extern MCC_Parser *parser;
extern int depth;

using namespace std;


class AST_Argument_;
class AST_ShortClassExp_;

class AST_Class_;

class AST_Equation_;
class AST_Equation_Equality_;
class AST_Equation_Connect_;
class AST_Equation_Call_;
class AST_Equation_If_;
class AST_Equation_For_;
class AST_Equation_Else_;
class AST_ForIndex_;

class AST_Component_ ;
class AST_Composition_;
class AST_CompositionElement_;
class AST_CompositionEqsAlgs_;
class AST_Declaration_;
class AST_Element_;
class AST_ExtendsClause_;
class AST_Element_ClassWrapper_;
class AST_ImportClause_ ;

class AST_Expression_ ;
class AST_Expression_String_;
class AST_Expression_Boolean;
class AST_Expression_Integer_;
class AST_Expression_Real_;
class AST_Expression_BinOp_;
class AST_Expression_Derivative_;
class AST_Expression_Null_;
class AST_Expression_If_;
class AST_Expression_ComponentReference_;
class AST_Expression_ComponentReference_;
class AST_Expression_UMinus_;
class AST_Expression_End_;
class AST_Expression_Call_;
class AST_Expression_BooleanNot_;
class AST_Expression_Output_;
class AST_Expression_If_ElseIf_;
class AST_Expression_Colon_;

class AST_Modification_;

class AST_Statement_;
class AST_Statement_Return_;
class AST_Statement_Break_;
class AST_Statement_Else_;

class AST_StoredDefinition_ ;

typedef bool                                 AST_Boolean;
typedef string                              *AST_String;
typedef int                                  AST_Integer;
typedef AST_Integer                          AST_Expression_Integer;
typedef double                               AST_Real;
typedef string                               AST_Type;
typedef AST_Argument_                       *AST_Argument;
typedef AST_ShortClassExp_                  *AST_ShortClassExp;
typedef AST_Class_                          *AST_Class;
typedef AST_Component_                      *AST_Component;
typedef AST_Composition_                    *AST_Composition;
typedef AST_CompositionElement_             *AST_CompositionElement;
typedef AST_CompositionEqsAlgs_             *AST_CompositionEqsAlgs;
typedef AST_Declaration_                    *AST_Declaration;
typedef AST_Equation_                       *AST_Equation;
typedef AST_Equation_Equality_              *AST_Equation_Equality;
typedef AST_Equation_Connect_               *AST_Equation_Connect;
typedef AST_Equation_Call_                  *AST_Equation_Call;
typedef AST_Equation_If_                    *AST_Equation_If;
typedef AST_Equation_For_                   *AST_Equation_For;
typedef AST_Equation_Else_                  *AST_Equation_Else;
typedef AST_ForIndex_                       *AST_ForIndex;
typedef AST_Expression_                     *AST_Expression;
typedef AST_Expression_BinOp_               *AST_Expression_BinOp;
typedef AST_Expression_Derivative_          *AST_Expression_Derivative;
typedef AST_Expression_Call_                *AST_Expression_Call;
typedef AST_Expression_End_                 *AST_Expression_End;
typedef AST_Expression_BooleanNot_          *AST_Expression_BooleanNot;
typedef AST_Expression_Null_                *AST_Expression_Null;
typedef AST_Expression_If_                  *AST_Expression_If;

typedef AST_Expression_If_ElseIf_           *AST_Expression_If_ElseIf;
typedef AST_Expression_Colon_               *AST_Expression_Colon;

typedef AST_Expression_String_              *AST_Expression_String; 
typedef AST_Expression_Real_                *AST_Expression_Real; 
typedef AST_Expression_UMinus_              *AST_Expression_UMinus; 
typedef AST_Expression_ComponentReference_  *AST_Expression_ComponentReference;
typedef AST_Expression_Output_              *AST_Expression_Output;
typedef AST_Element_                        *AST_Element;
typedef AST_Element_ClassWrapper_           *AST_Element_ClassWrapper;
typedef AST_ExtendsClause_                  *AST_ExtendsClause;
typedef AST_ImportClause_                   *AST_ImportClause;
typedef AST_Modification_                   *AST_Modification;
typedef AST_StoredDefinition_               *AST_StoredDefinition; 
typedef AST_Statement_                      *AST_Statement;
typedef AST_Statement_Return_               *AST_Statement_Return;
typedef AST_Statement_Break_                *AST_Statemnet_Break;
typedef AST_Statement_Else_                 *AST_Statement_Else;
typedef int                                  AST_TypePrefix;
typedef int                                  AST_ClassPrefix;

/* Lists */
typedef std::list<AST_String>               *AST_StringList;
typedef std::list<AST_Class>                *AST_ClassList;
typedef std::list<AST_Element>              *AST_ElementList;
typedef std::list<AST_Equation>             *AST_EquationList;
typedef std::list<AST_Expression>           *AST_ExpressionList;
typedef std::list<AST_ExpressionList>       *AST_ExpressionListList;
typedef std::list<AST_CompositionElement>   *AST_CompositionElementList;
typedef std::list<AST_Declaration>          *AST_DeclarationList;
typedef std::list<AST_Component>            *AST_ComponentList;
typedef std::list<AST_ForIndex>             *AST_ForIndexList;
typedef std::list<AST_Equation_Else>        *AST_Equation_ElseList;
typedef std::list<AST_Statement>            *AST_StatementList;
typedef std::list<AST_Statement_Else>       *AST_Statement_ElseList;
typedef std::list<AST_Argument>             *AST_ArgumentList;

/* Iterators */
typedef std::list<AST_Class>::iterator               AST_ClassListIterator;
typedef std::list<AST_CompositionElement>::iterator  AST_CompositionElementListIterator;
typedef std::list<AST_Component>::iterator           AST_ComponentListIterator;
typedef std::list<AST_Declaration>::iterator         AST_DeclarationListIterator;
typedef std::list<AST_Element>::iterator             AST_ElementListIterator;
typedef std::list<AST_Equation>::iterator            AST_EquationListIterator;
typedef std::list<AST_Expression>::iterator          AST_ExpressionListIterator;
typedef std::list<AST_String>::iterator              AST_StringListIterator;
typedef std::list<AST_Statement>::iterator           AST_StatementListIterator;

typedef std::list<AST_Expression>::reverse_iterator  AST_ExpressionListReverseIterator;

extern AST_StoredDefinition root;

/* Enums */
enum ElementType { ELNONE, COMPONENT, IMPORT, EXTENDS, ELCLASS };
enum EquationType { EQNONE, EQEQUALITY, EQCONNECT,EQCALL, EQFOR, EQWHEN, EQIF };
enum ExpressionType { EXPNONE ,EXPCOMPREF, EXPBINOP , EXPDERIVATIVE , EXPNULL, EXPEND, EXPIF, EXPCALL, EXPELSEIF, EXPCOLON, EXPUMINUS, EXPBOOLEAN, EXPSTRING, EXPREAL, EXPINTEGER, EXPBOOLEANNOT, EXPOUTPUT };
enum BinOpType {BINOPLOWER, BINOPLOWEREQ, BINOPGREATER, BINOPGREATEREQ, BINOPCOMPNE, BINOPCOMPEQ, BINOPDIV, BINOPELDIV, BINOPMULT, BINOPELMULT, BINOPADD, BINOPELADD,
                BINOPSUB, BINOPELSUB, BINOPEXP, BINOPELEXP, BINOPAND, BINOPOR };



/* Constructors */

/* Argument modification */
AST_ArgumentList newAST_ArgumentList();
AST_Argument newAST_Redeclaration(AST_Boolean, AST_Boolean, AST_Argument); 
AST_Argument newAST_ShortClass(AST_ClassPrefix,AST_String, AST_ShortClassExp);
AST_ShortClassExp newAST_ShortClassExp(AST_TypePrefix, AST_String, AST_ExpressionList, AST_ArgumentList);
AST_ShortClassExp newAST_ShortClassExp_Enum(AST_StringList); 
AST_Argument AST_ArgumentSet(AST_Boolean, AST_Boolean, AST_Argument);
AST_Argument newAST_ElementModification(AST_String, AST_Modification);
AST_Argument AST_ArgumentSetReplaceable(AST_Argument);

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
#define _S(s) newAST_String(s)


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
AST_Component newAST_Component(AST_DeclarationList cl, AST_String type, AST_TypePrefix);
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
AST_Declaration newAST_Declaration(AST_String);
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
AST_Expression_ComponentReference newAST_Expression_ComponentReference(AST_String);
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
AST_Expression newAST_Expression_Call(AST_String,AST_String);
AST_Expression newAST_Expression_DotCall(AST_String,AST_String);
AST_Expression newAST_Expression_BooleanNot(AST_Expression);
AST_Expression newAST_Expression_ElseIf(AST_Expression,AST_Expression);
AST_Expression newAST_Expression_NamedArgument(AST_String, AST_Expression);
AST_Expression newAST_Expression_FunctionCallArgs(AST_ExpressionList);
AST_Expression newAST_Expression_Function(AST_String, AST_ExpressionList);
AST_Expression newAST_Expression_OutputExpressions(AST_ExpressionList);
AST_Expression newAST_Expression_Range(AST_Expression, AST_Expression);
AST_Expression newAST_Expression_FunctionArguments(AST_ExpressionList);
AST_Expression newAST_BracketExpList(AST_ExpressionListList);

/* Equations */
AST_EquationList newAST_EquationList();
AST_Equation newAST_Equation_Equality(AST_Expression, AST_Expression);
AST_Equation newAST_Equation_Connect(AST_Expression_ComponentReference, AST_Expression_ComponentReference);
AST_Equation newAST_Equation_If(AST_Expression e, AST_EquationList eql, AST_Equation_ElseList, AST_EquationList);
AST_Equation newAST_Equation_For(AST_ForIndexList, AST_EquationList eql);
AST_Equation newAST_Equation_When(AST_Expression,AST_EquationList, AST_Equation_ElseList);
AST_Equation newAST_Equation_Call(AST_Expression e);
AST_ForIndex newAST_ForIndex(AST_String, AST_Expression);
AST_ForIndexList newAST_ForIndexList();
AST_Equation_ElseList newAST_Equation_ElseList();
AST_Equation_Else newAST_Equation_Else(AST_Expression, AST_EquationList);

/* Modification */
AST_Modification newAST_Modification(AST_ArgumentList,AST_Expression); 
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
ostream & operator<<(ostream &os , const AST_StoredDefinition &sd ) ;
ostream & operator<<(ostream &os , const AST_Composition &c ) ;
ostream & operator<<(ostream &os , const AST_Class &c ) ;
ostream & operator<<(ostream &os , const AST_Expression &e ) ;
ostream & operator<<(ostream &os , const AST_String &s ) ;
ostream & operator<<(ostream &os , const AST_Equation &e ) ;
ostream & operator<<(ostream &os , const AST_Statement &s ) ;
ostream & operator<<(ostream &os , const AST_CompositionElement &ce ) ;
ostream & operator<<(ostream &os , const AST_Expression_ComponentReference &cr);
/* List uitls */

#define current(it) (*it)
#define foreach(it,list) for (it=list->begin();it!=list->end();it++)
#define foreachReverse(it,list) for (it=list->rbegin();it!=list->rend();it++)

template <typename T1>
list<T1> * AST_ListAppend(list<T1> *l, T1 e) { l->push_back(e); return l; }
template <typename T1>
list<T1> * AST_ListPrepend(list<T1> *l, T1 e) { l->push_front(e); return l; }
template <typename T1>
T1 AST_ListFirst(list<T1> *l) {return l->front();}
#endif
