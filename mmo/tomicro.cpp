
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

#include <mmo/tomicro.h>

#include <iostream>
using namespace std;

MMO_ToMicroModelica_::MMO_ToMicroModelica_ (MMO_Class * c): _c(c) {} ;


void MMO_ToMicroModelica_::transform()
{
	AST_EquationListIterator eqit;
	list<AST_EquationListIterator> * del = new list<AST_EquationListIterator>();
	
    foreach(eqit,_c->getEquations()) {		
		AST_Equation eq = current(eqit);
				
		switch (eq->equationType()) 
		{
			case EQEQUALITY:
			{
				AST_Equation _e = toMicro_eq_equality(eq->getAsEquality());
				
				if (_e != NULL)  current(eqit) = _e;
				else AST_ListAppend(del,eqit);
				break;
			}
			
			case EQNONE:
			{	
				break;
			}
			
			case EQWHEN:
			{
				AST_Equation_When when = eq->getAsWhen();
				_c->addStatement( toMicro_eq_when(when));
				AST_ListAppend(del,eqit);
				break;
			}
			
			case EQIF:
			{
				toMicro_eq_if(eq->getAsIf());
				AST_ListAppend(del,eqit);
				break;
			}	
			
			default:
				cerr << "Error: "  << eq->equationType() << endl;
				throw "Not implemented yet!!";
			
		}
			
	}
	
	list<AST_EquationListIterator>::iterator delIt;
	foreach(delIt,del) _c->getEquations()->erase(current(delIt));

	
}


AST_String MMO_ToMicroModelica_::new_label()
{
	static int n = 0 ;
	stringstream ret;
	ret << "z_" << n ;
	n++;
	return _S(ret.str());
}

AST_Expression negar_cond(AST_Expression cond)
{
	AST_Expression_BinOp b = cond->getAsBinOp();
	switch (b->binopType())
	{
		case BINOPLOWER: 
			return newAST_Expression_BinOp(b->left(),b->right(), BINOPGREATER );
		case BINOPLOWEREQ: 
			return newAST_Expression_BinOp(b->left(),b->right(), BINOPGREATEREQ );
		case BINOPGREATER: 
			return newAST_Expression_BinOp(b->left(),b->right(), BINOPLOWER );
		case BINOPGREATEREQ: 
			return newAST_Expression_BinOp(b->left(),b->right(), BINOPLOWEREQ );
		case BINOPCOMPNE: 
			return newAST_Expression_BinOp(b->left(),b->right(), BINOPCOMPEQ );
		case BINOPCOMPEQ: 
			return newAST_Expression_BinOp(b->left(),b->right(), BINOPCOMPNE );
	}
}


MMO_Statement MMO_ToMicroModelica_::make_when(AST_Expression cond , AST_Expression_ComponentReference var)
{
	AST_Statement e1 = newAST_Statement_Assign(var , I(1) );
	AST_Statement e2 = newAST_Statement_Assign(var , I(0) );
	
	AST_StatementList l1 = new list < AST_Statement > ();
	AST_ListAppend(l1, e1 );
	
	AST_StatementList l2 = new list < AST_Statement > ();
	AST_ListAppend(l2,e2 );
	
	AST_Statement_Else  _else = newAST_Statement_Else ( negar_cond(cond) , l2 ); 
	AST_Statement_ElseList elList = newAST_Statement_ElseList();
	AST_ListAppend(elList,_else );

	return newAST_Statement_When( cond , l1 , elList );
	
}


AST_Expression find_equation(AST_Expression e, AST_EquationList eqList ) 
{
	AST_EquationListIterator eqit;
	foreach( eqit , eqList ) 
		if ( current(eqit)->equationType()  == EQEQUALITY ) {
			AST_Equation_Equality eq = current(eqit)->getAsEquality();
			if ( e->print() ==  eq->left()->print() )
				return eq->right();
	}
	return NULL;
	
}

MMO_Equation MMO_ToMicroModelica_::toMicro_eq_equality(AST_Equation_Equality eq)
{
	AST_Expression r = eq->right();
	switch (r->expressionType())
	{
		case EXPBINOP:
		{
			AST_Expression_BinOp b = r->getAsBinOp();
			switch (b->binopType()) 
			{
				case BINOPLOWER: 
				case BINOPLOWEREQ: 
				case BINOPGREATER: 
				case BINOPGREATEREQ: 
				case BINOPCOMPNE: 
				case BINOPCOMPEQ:
				{
					// Ver esta precondicion (si o si component ref)	
					_c->addStatement(make_when( b, eq->left()->getAsComponentRef() ));
					return NULL;
				
				} 
		
			}
			break;
		}
		
		case EXPCALL:
		{
			// Analizar sample y otras llamadas a funcion!!!
			return NULL;
		}
			
	}
	AST_Expression _r = toMicro_exp(eq->right());
	return newAST_Equation_Equality( eq->left() , _r );
}


AST_Expression MMO_ToMicroModelica_::toMicro_exp(AST_Expression e)
{
	switch (e->expressionType())
	{
		case EXPBINOP:
		{
			AST_Expression_BinOp b = e->getAsBinOp();
			switch (b->binopType()) 
			{
				case BINOPLOWER: 
				case BINOPLOWEREQ: 
				case BINOPGREATER: 
				case BINOPGREATEREQ: 
				case BINOPCOMPNE: 
				case BINOPCOMPEQ:
				{
					AST_String name = new_label();
					_c->addVariable( name , _S("Real"));	
					AST_Expression_ComponentReference cr = newAST_Expression_ComponentReferenceExp (name)->getAsComponentRef();
					AST_Expression _b = newAST_Expression_BinOp( toMicro_exp(b->left()) , toMicro_exp(b->right()) ,b->binopType()  );
					_c->addStatement(make_when( _b, cr ));
					return cr;
					
				}
				
				case BINOPAND:
				{
					return MULT( toMicro_exp(b->left()) , toMicro_exp(b->right()) );
				}
				
				case BINOPOR:
				{
					return SUB(I(1) , MULT( SUB(I(1) , toMicro_exp(b->left())) , SUB(I(1) ,toMicro_exp(b->right()) ) ) ) ;
				}	
				
				default:
				{
					return newAST_Expression_BinOp( toMicro_exp(b->left()) , toMicro_exp(b->right()) ,b->binopType()  );
				}
				 
			}
		}
		
		case EXPUMINUS:
		{
			AST_Expression_UMinus u = e->getAsUMinus();
			return newAST_Expression_UnaryMinus( toMicro_exp(u->exp()) );
		}
		
		case EXPOUTPUT :
		{
			AST_Expression_Output b = e->getAsOutput();
			AST_ExpressionList ls = new list < AST_Expression > ();
			AST_ListAppend(ls,toMicro_exp(b->expressionList()->front() ) )	;
			return newAST_Expression_OutputExpressions(ls);
		}
		
		case EXPCALL:
		{
			AST_Expression_Call call = e->getAsCall();
			return call;
		}
		
		case EXPCOMPREF:
			return e;
			
		case EXPBOOLEANNOT:	
		{
			AST_Expression_BooleanNot no = e->getAsBooleanNot();
			return SUB(I(1) , toMicro_exp( no->exp() ) ) ;
		}	
		
		case EXPBOOLEAN:
		{
			AST_Expression_Boolean b = e->getAsBoolean();
			if (b->value()) return I(1);
			else return I(0);
		}	
			
		case EXPIF:
		{
			AST_Expression_If i = e->getAsIf();
			if ( ! i->elseif_list()->empty() ) {
					
				AST_ExpressionListReverseIterator it;
				AST_Expression iff;
				AST_ExpressionList iflist = i->elseif_list();
				
				foreachReverse(it,iflist) {
					if (current(it)->expressionType() == EXPELSEIF) {
						AST_Expression_If_ElseIf elif = current(it)->getAsElseIf();					
						if  (it == iflist->rbegin())
							iff = newAST_Expression_If( elif->condition() , elif->then() , newAST_ExpressionList() ,i->else_exp() );					
						else 	
							iff = newAST_Expression_If( elif->condition() , elif->then() , newAST_ExpressionList() , iff );
					}
				}
				iff = newAST_Expression_If( i->condition() , i->then() , newAST_ExpressionList() , iff );
				i = iff->getAsIf();
			}
			
			AST_Expression eq1 = toMicro_exp(i->then());
			AST_Expression eq2 = toMicro_exp(i->else_exp());
			AST_Expression cond = toMicro_exp(i->condition());
			return ADD( MULT( cond , eq1 )  ,  MULT ( SUB(I(1), cond )   , eq2 )  );
			
		}	
			
			
		default:
			return e;
		
		
	}
	
}


AST_Expression MMO_ToMicroModelica_::makeCondition(AST_ExpressionList ls , int n )
{
	AST_ExpressionListIterator exit =  ls->begin();
	int i;
	if (n==0) return  current(exit);
	AST_Expression e = UMENOS(current(exit)) ;
	exit++;
	for(i = 1; i < n; i++ ){
		e = MULT( e , UMENOS(  current(exit) ));
		exit++;
	}
	if (n < ls->size())
		e = MULT( e , current(exit));
	return e;
	
}

AST_Expression MMO_ToMicroModelica_::makeEquation(AST_EquationList ls , AST_ExpressionList cond)
{
	int i = 0;
	AST_Expression e1,e2;
	AST_EquationListIterator eqit = ls->begin();
	e1 = MULT( toMicro_exp(current(eqit)->getAsEquality()->left())   , makeCondition(cond,0) );
	e2 = MULT( toMicro_exp(current(eqit)->getAsEquality()->right())  , makeCondition(cond,0) );
	eqit++;
	for (i = 1; i < ls->size(); i ++ ){
		e1 = ADD(e1,MULT( toMicro_exp(current(eqit)->getAsEquality()->left())   , makeCondition(cond,i) ));
		e2 = ADD(e2,MULT( toMicro_exp(current(eqit)->getAsEquality()->right())  , makeCondition(cond,i) ));
		eqit++;
	}
	_c->addEquation(newAST_Equation_Equality(e1,e2));
}


/*
 * 
 * CAMBIAR ESTE IF POR EL OTRO NUEVO
 * PRECONDICION: MISMA CANTIDAD DE ECUACIONES EN CADA RAMA
*/
void MMO_ToMicroModelica_::toMicro_eq_if(AST_Equation_If iff)
{
	AST_ExpressionList condList = newAST_ExpressionList();
	AST_ListAppend(condList,toMicro_exp(iff->condition()));
	
	/*  Lista de condiciones */ 
	if ( ! iff->equationElseIf()->empty() ) {
		AST_Equation_ElseListIterator elseit;
		foreach(elseit, iff->equationElseIf()) AST_ListAppend(condList, toMicro_exp(current(elseit)->condition()) );
	}
	int nro =  iff->equationList()->size();
	
	/*  Re ordeno las ecuaciones */ 
	AST_EquationList * ls = new AST_EquationList[nro];
	for(int i = 0; i < nro;i++) ls[i] = newAST_EquationList();
	
	AST_EquationListIterator eqit;
	int k = 0;
	foreach (eqit, iff->equationList()) AST_ListAppend( ls[k++] , current(eqit) );
	
	if ( ! iff->equationElseIf()->empty() ) {
		AST_Equation_ElseListIterator elseit;
		foreach(elseit, iff->equationElseIf()) {
			AST_Equation_Else elseIf =  current(elseit);
			k = 0;
			foreach( eqit , elseIf->equations() ) AST_ListAppend( ls[k++] , current(eqit) );		
		}
	}
	k=0;
	foreach (eqit, iff->equationElseList()) AST_ListAppend( ls[k++] , current(eqit) );
	
	/* Ahora transformo y creo */
	for(k= 0;k < nro;k++) makeEquation(ls[k],condList );
	
	 
	
	
}

/*
 * 	FALTA VER SAMPLE Y LAS VARIABLES STATES
 * 
 */
 
AST_Expression MMO_ToMicroModelica_::whenCondition(AST_Expression e, AST_StatementList ls )
{
	switch (e->expressionType())
	{
		case EXPBINOP:
		{
			AST_Expression_BinOp b = e->getAsBinOp();
			return newAST_Expression_BinOp(whenCondition(b->left(),ls),whenCondition(b->right(),ls) , b->binopType());
		}
		
		case EXPUMINUS:
		{
			AST_Expression_UMinus u = e->getAsUMinus();
			return newAST_Expression_UnaryMinus( whenCondition(u->exp(),ls));
		}
		
		case EXPBOOLEANNOT:
		{
			AST_Expression_BooleanNot no = e->getAsBooleanNot();
			return newAST_Expression_BooleanNot( whenCondition( no->exp() ,ls) );
		}
		
		case EXPOUTPUT:
		{
			AST_Expression_Output b = e->getAsOutput();
			AST_ExpressionList lss = new list < AST_Expression > ();
			AST_ListAppend(lss,whenCondition(b->expressionList()->front() , ls ) )	;
			return newAST_Expression_OutputExpressions(lss);
		}
		
		case EXPCALL:
		{
			AST_Expression_Call call = e->getAsCall();
			if ( * call->name() == "sample") {
				AST_String name = new_label();   // TNEXT
				_c->addVariable( name , _S("Real"));	
				AST_Expression_ComponentReference cr = newAST_Expression_ComponentReferenceExp (name)->getAsComponentRef();
				
				AST_Expression per =   current (  ++call->arguments()->begin()   );
				
				AST_ListAppend(ls ,  newAST_Statement_Assign(cr, ADD(  cr ,per )  ) );
				return GREATER( VAR(_S("time")) , cr );
			}
			if ( * call->name() == "initial") {
				return GREATER( VAR(_S("time")) , I(0) );
			}
			return call;
		}
		default:
			return e;
	}	
} 
 

MMO_Statement MMO_ToMicroModelica_::toMicro_eq_when (AST_Equation eq) 
{
	switch (eq->equationType()) {
		case EQEQUALITY:
		{ 
			AST_Equation_Equality _e =  eq->getAsEquality();
			if (_e->left()->expressionType() == EXPCOMPREF)
			{
				AST_Expression_ComponentReference cf = _e->left()->getAsComponentRef(); 
				VarInfo *varInfo = _c->getVarSymbolTable()->lookup( * (cf->names()->front()));
				if (varInfo->isState()) {
					AST_ExpressionList ls = newAST_ExpressionList(); 
					AST_ListAppend(ls, (AST_Expression)cf );
					AST_ListAppend(ls, _e->right() );
					return newAST_Statement_Assign(AST_Expression_ComponentReference(_S("reinit")), newAST_Expression_FunctionCallArgs(ls) );
				} else 
					return newAST_Statement_Assign( cf  , _e->right() );
			}
			//if (_e->left()->expressionType() == EXPOUTPUT)	
			//	return 
		}
	
		case EQWHEN:
		{
			AST_Equation_When when = eq->getAsWhen();
			AST_Statement_ElseList elseList = newAST_Statement_ElseList();
			if (when->equationElseWhen()->size() > 0 ) {
				AST_Equation_ElseListIterator elit;
				foreach(elit, when->equationElseWhen()){
					AST_Equation_Else qelse = current(elit); 
					
					AST_StatementList stmList = newAST_StatementList();
					AST_EquationListIterator eqit;
					foreach(eqit, qelse->equations()) AST_ListAppend(stmList,toMicro_eq_when(current(eqit)) );
					
					AST_Expression _cond = whenCondition(qelse->condition(), stmList);
					AST_ListAppend(elseList,newAST_Statement_Else( _cond , stmList   ) );
				}	
			}
			
			AST_StatementList stmList = newAST_StatementList();
			AST_EquationListIterator eqit;
			foreach(eqit, when->equationList()) AST_ListAppend(stmList,toMicro_eq_when(current(eqit)) );
			
			AST_Expression _cond = whenCondition(when->condition(), stmList);
			
			return newAST_Statement_When( _cond , stmList , elseList);  
		}
		
		case EQIF:
		{
			AST_Equation_If i = eq->getAsIf();
			AST_Statement_ElseList elseList = newAST_Statement_ElseList();
			if (i->equationElseIf()->size() > 0 ) {
				AST_Equation_ElseListIterator elit;
				foreach(elit, i->equationElseIf()){
					AST_Equation_Else qelse = current(elit); 
					
					AST_StatementList stmList = newAST_StatementList();
					AST_EquationListIterator eqit;
					foreach(eqit, qelse->equations()) AST_ListAppend(stmList,toMicro_eq_when(current(eqit)) );
					
					AST_ListAppend(elseList,newAST_Statement_Else( qelse->condition() , stmList   ) );
				}	
			}
			
			AST_StatementList stmList = newAST_StatementList();
			AST_EquationListIterator eqit;
			foreach(eqit, i->equationList()) AST_ListAppend(stmList,toMicro_eq_when(current(eqit)) );
			
			AST_StatementList stmElseList = newAST_StatementList();
			foreach(eqit, i->equationElseList()) AST_ListAppend(stmElseList,toMicro_eq_when(current(eqit)) );
			
			return newAST_Statement_If( i->condition() , stmList , elseList,stmElseList); 
			
		}
	
		case EQFOR:
		{
			AST_Equation_For f = eq->getAsFor();
			AST_StatementList stmList = newAST_StatementList();
			AST_EquationListIterator eqit;
			foreach(eqit, f->equationList()) AST_ListAppend(stmList,toMicro_eq_when(current(eqit)) );
			return newAST_Statement_For(f->forIndexList() , stmList );
		}
		
	}	
}
