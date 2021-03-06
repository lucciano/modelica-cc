
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
#include <mmo/mmo_class.h>
#include <ast/equation.h>
#include <ast/statement.h>
#include <util/type.h>
#include <util/ast_util.h>
#include <set>
#include <stack>

#ifndef TOMICRO_H
#define TOMICRO_H


DEFINE_TYPE(MMO_ToMicroModelica);

//#define VAR(v)			newAST_Expression_ComponentReferenceExp(v)
//#define GREATER(l,r) 	newAST_Expression_BinOp(l, r, BINOPGREATER )
//#define LOWER(l,r) 		newAST_Expression_BinOp(l, r, BINOPLOWER )
//#define ADD(l,r) 		newAST_Expression_BinOp(l, r, BINOPADD )
//#define MULT(l,r) 		newAST_Expression_BinOp(l, r, BINOPMULT )
//#define SUB(l,r) 		newAST_Expression_BinOp(l, r, BINOPSUB )
//#define I(n) 			newAST_Expression_Integer(n)
//#define R(n) 			newAST_Expression_Real(n)
//#define PA(e)       	newAST_Expression_OutputExpressions(e)
//#define UMENOS(e)   	SUB( I(1) , e )

#define IS_COMPARE(X)	((X)->expressionType()==EXPBINOP && ( (X)->getAsBinOp()->binopType() >= BINOPLOWER  && (X)->getAsBinOp()->binopType() <= BINOPCOMPEQ  ))
#define IS_CREF(X)    	((X)->expressionType()==EXPCOMPREF)

typedef std::map<string, AST_Expression> 		IndexMap_;
typedef std::map<string, AST_Expression> 		* IndexMap;



class MMO_ToMicroModelica_ {
public:
	MMO_ToMicroModelica_(MMO_Class  c );
	
	void transform();
	
private:
	MMO_Class  _c;
	AST_String  new_label();
	PreSet _pre;
	
	stack<MMO_StatementList> initialFrame;
	
	void transformEqList(AST_EquationList  , AST_StatementList,IndexMap );
	
	/*  Trasforma las Equaciones de Igualdad*/
	MMO_Equation 		toMicro_eq_equality(AST_Equation_Equality , MMO_StatementList,IndexMap); 
	
	/*  Transforma las expresiones */
	AST_Expression 		toMicro_exp(AST_Expression , AST_StatementList,IndexMap);
	 
	/* Crea un Statement_When  para la discontinuidad */ 
	MMO_Statement 		make_when(AST_Expression  , AST_Expression_ComponentReference );
	MMO_Statement 		make_whenABS(AST_Expression  , AST_Expression_ComponentReference );
	MMO_Statement 		make_whenMin(AST_Expression, AST_Expression  , AST_Expression_ComponentReference );
	MMO_Statement 		make_whenMax(AST_Expression, AST_Expression  , AST_Expression_ComponentReference );
	MMO_Statement 		make_whenSING(AST_Expression  , AST_Expression_ComponentReference );
	AST_Expression		create_Variable();
	 
	/* Procesa la condicion del When */
	AST_Expression 		whenCondition(AST_Expression , AST_StatementList  );
	AST_Expression 		whenConditionTransform(AST_Expression , AST_StatementList  );
	
	 /*  Transforma las equaciones WHEN */ 
	void				toMicro_eq_when (AST_Equation,MMO_StatementList );
	 
	/* Genera el if para statement initial */
	MMO_Statement 		make_if(AST_Expression  , AST_Expression_ComponentReference var);
	MMO_Statement 		make_ifABS(AST_Expression  , AST_Expression_ComponentReference var);
	MMO_Statement 		make_ifMin(AST_Expression, AST_Expression  , AST_Expression_ComponentReference var);
	MMO_Statement 		make_ifMax(AST_Expression, AST_Expression  , AST_Expression_ComponentReference var);
	MMO_Statement 		make_ifSING(AST_Expression  , AST_Expression_ComponentReference var);
	/* Niega las primeras i-th condiciones (Utilizado en el IF )*/
	AST_Expression 		makeCondition(AST_ExpressionList  , unsigned int  );
	
	/* Genera una Equacion desde un listado de equaciones y Condiciones (Utilizado en el IF ) */
	AST_Equation 		makeEquation(AST_EquationList  , AST_ExpressionList ,MMO_StatementList,IndexMap ); 
	
    /*  Transforma las equaciones IF */ 
	AST_EquationList 	toMicro_eq_if(AST_Equation_If,MMO_StatementList,IndexMap );
	
	/*  Analiza los indices usado en el for */ 
	IndexMap viewIndex(IndexMap , AST_ForIndexList );
	
	/*  Analiza si la expresion depende de una variable */
	bool IndexAccess(AST_Expression e, string i );
	
	/*  Transforma las equaciones For */ 
	MMO_Equation		toMicro_eq_for(AST_Equation_For,MMO_StatementList,IndexMap);
	
	/* Cambia las constante TRUE Y FALSE de las modificationes */
	AST_Modification ChangeModifications(AST_Modification m);
	
	/* Cambia un tipo a REAL */ 
	Type ChangeToReal(Type m);
	
	/* Analiza los statements */
	void checkStatement(MMO_StatementList ls);
	
	/*  Aplica la funcion pre a quien corresponda */
	void ChangePre(AST_EquationList eqList);
};

MMO_ToMicroModelica newMMO_ToMicroModelica(MMO_Class  c );

#endif
