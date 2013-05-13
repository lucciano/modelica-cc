
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
		
		case EXPOUTPUT :
		{
			AST_Expression_Output b = e->getAsOutput();
			AST_ExpressionList ls = new list < AST_Expression > ();
			AST_ListAppend(ls,toMicro_exp(b->expressionList()->front() ) )	;
			return newAST_Expression_OutputExpressions(ls);
		}
		
		case EXPCALL:
		{
			
			
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



/*
 * 
 * CAMBIAR ESTE IF POR EL OTRO NUEVO
 * 
*/
void MMO_ToMicroModelica_::toMicro_eq_if(AST_Equation_If iff)
{
	AST_EquationListIterator eqit;
	foreach( eqit , iff->equationList() )
	{
		if ( current(eqit)->equationType()  != EQEQUALITY ) throw "No es una ecuacion!!";
		AST_Equation_Equality e = current(eqit)->getAsEquality();
		AST_Expression find = find_equation(e->left() , iff->equationElseList());
	
		if (find == NULL ) throw "No se encontro la ecuacion!!";
		
		AST_ExpressionList elseIfList = newAST_ExpressionList() ;
		
		if ( ! iff->equationElseIf()->empty() ) {
			AST_Equation_ElseListIterator elseit;
			foreach(elseit, iff->equationElseIf()) {
				AST_Equation_Else elseIf =  current(elseit);
				
				AST_Expression _find = find_equation(e->left() , elseIf->equations());
				if (_find == NULL ) throw "No se encontro la ecuacion!!";
				AST_ListAppend(elseIfList,newAST_Expression_ElseIf(elseIf->condition() , _find) );
				
				
			}
			
		}
		
		AST_Expression expIf = newAST_Expression_If(iff->condition() , e->right(), elseIfList , find );
		_c->addEquation( newAST_Equation_Equality( e->left(), expIf  ));
		
	}
	return ;
}


/*
 * 	FALTA VER SAMPLE Y LAS VARIABLES STATES
 * 
 */
 

MMO_Statement MMO_ToMicroModelica_::toMicro_eq_when (AST_Equation eq) 
{
	switch (eq->equationType()) {
		case EQEQUALITY:
		{ 
			AST_Equation_Equality _e =  eq->getAsEquality();
			if (_e->left()->expressionType() == EXPCOMPREF)
				return newAST_Statement_Assign(  _e->left()->getAsComponentRef() , _e->right() );
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
					
					AST_ListAppend(elseList,newAST_Statement_Else( qelse->condition() , stmList   ) );
				}	
			}
			
			AST_StatementList stmList = newAST_StatementList();
			AST_EquationListIterator eqit;
			foreach(eqit, when->equationList()) AST_ListAppend(stmList,toMicro_eq_when(current(eqit)) );
			
			return newAST_Statement_When( when->condition() , stmList , elseList);  
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
