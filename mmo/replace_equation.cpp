
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

#include <mmo/replace_equation.h>

#include <iostream>
using namespace std;

MMO_Replace_Equation_::MMO_Replace_Equation_ (MMO_Class * c): _c(c) {} ;

void MMO_Replace_Equation_::replace() 
{
	AST_EquationListIterator eqit;
	AST_Expression ex;
	list<AST_EquationListIterator> * del = new list<AST_EquationListIterator>();
    foreach(eqit,_c->getEquations()) {
		AST_Equation eq = current(eqit);

				
		switch (eq->equationType()) {
			case EQEQUALITY:
			{
				AST_Equation_Equality _e =  eq->getAsEquality();
				Type t = _c->getExpresionType(  _e->right());
				
				// Just for Now
				switch (t->getType()) {
					case TYINTEGER:
					case TYREAL:
						ex = replace_real(_e->right());
						current(eqit) = newAST_Equation_Equality( _e->left() , ex) ;
						break;
					case TYSTRING:
						break;
					case TYBOOLEAN:
						ex = replace_bool( _e->left()->getAsComponentRef()  ,  _e->right());
						if (ex == NULL) del->push_back(eqit);
						else current(eqit) = newAST_Equation_Equality( _e->left() , ex) ;
							
						break;
					default:
						throw "This type is not suported";
				}
				break;
			}	
				
			case EQNONE:
			{	
				break;
			}
			
			case EQWHEN:
			{
				AST_Equation_When when = eq->getAsWhen();
				_c->addStatement( replace_when_eq (when)  );
				del->push_back(eqit);
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


MMO_Statement MMO_Replace_Equation_::make_when(AST_Expression cond , AST_Expression_ComponentReference var)
{
	AST_Statement e1 = newAST_Statement_Assign(var , I(1) );
	AST_Statement e2 = newAST_Statement_Assign(var , I(0) );
	
	AST_StatementList l1 = new list < AST_Statement > ();
	l1->push_back( e1 );
	
	AST_StatementList l2 = new list < AST_Statement > ();
	l2->push_back( e2 );
	
	// FALTA NEGAR LA CONDICION!!!!
	
	AST_Statement_Else  _else = newAST_Statement_Else ( cond , l2 ); 
	AST_Statement_ElseList elList = newAST_Statement_ElseList();
	elList->push_back ( _else );

	return newAST_Statement_When( cond , l1 , elList );
	
}


AST_Expression MMO_Replace_Equation_::replace_bool(AST_Expression_ComponentReference v , AST_Expression e)
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
					_c->addStatement(make_when( b, v ));
					return NULL;
				}
				
				case BINOPAND:
				case BINOPOR:
				{
					return generate_condition(e);
				}
				
				default:
					throw "Binop no soportado. No deberia pasar";
			
			}
			
		}
		
		case EXPBOOLEANNOT:
		{
			return e;
		}
		
		case EXPOUTPUT :
		{
			AST_Expression_Output b = e->getAsOutput();
			AST_ExpressionList ls = new list < AST_Expression > ();
			ls->push_back(replace_bool(v, b->expressionList()->front() )    )	;
			return newAST_Expression_OutputExpressions(ls);
		}  
		  
		case EXPIF:
		{   
			AST_Expression_If i = e->getAsIf();
			AST_Expression eq1 = replace_bool(v,i->then());
			AST_Expression eq2 = replace_bool(v,i->else_exp());
			
			return replace_condition(i->condition() , eq1 , eq2);
					
		}
		
		default:
			return e;
		
	}  
	return e;
}

AST_Expression MMO_Replace_Equation_::replace_real(AST_Expression e)
{
	switch (e->expressionType()) 
	{
		case EXPBINOP:
		{
			AST_Expression_BinOp b = e->getAsBinOp();
			return newAST_Expression_BinOp(replace_real(b->left()),replace_real(b->right()), b->binopType());
			
		}
		
		case EXPUMINUS: 
		{
			AST_Expression_UMinus m = e->getAsUMinus();
			return newAST_Expression_UnaryMinus(replace_real(m->exp()));
		}
		
		case EXPOUTPUT :
		{
			AST_Expression_Output b = e->getAsOutput();
			AST_ExpressionList ls = new list < AST_Expression > ();
			ls->push_back(replace_real( b->expressionList()->front() )    )	;
			return newAST_Expression_OutputExpressions(ls);
		}  
		  
		case EXPIF:
		{   
			AST_Expression_If i = e->getAsIf();
			AST_Expression eq1 = replace_real(i->then());
			AST_Expression eq2 = replace_real(i->else_exp());
			
			return replace_condition(i->condition() , eq1 , eq2);
					
		}
		
		default:
			return e;
		
	}  
}

AST_Expression MMO_Replace_Equation_::generate_condition( AST_Expression c )
{
		switch (c->expressionType())
	{
		case EXPOUTPUT :
		{
			AST_Expression_Output b = c->getAsOutput();
			return generate_condition( b->expressionList()->front()); 
		}  
		
		case EXPBINOP:
		{
			AST_Expression_BinOp b = c->getAsBinOp();
			switch (b->binopType()) {
				case BINOPAND:
				{
					return MULT( generate_condition(b->left()) , generate_condition(b->right()) );
				}
				
				case BINOPOR:
				{
					return SUB(I(1) , MULT( SUB(I(1) , generate_condition(b->left())) , SUB(I(1) ,generate_condition(b->right()) ) ) ) ;
				}	
				default: 
					throw "Error!! No deberia pasar esto";
			}
		}		
		case EXPCOMPREF:
		{
			return c;
		}
		case EXPBOOLEANNOT:	
		{
			AST_Expression_BooleanNot no = c->getAsBooleanNot();
			return SUB(I(1) , generate_condition( no->exp() ) ) ;
		}	
		default:
			throw "Error!! No deberia pasar esto";
	}
}

AST_Expression MMO_Replace_Equation_::replace_condition(AST_Expression c , AST_Expression eq1, AST_Expression eq2)
{
	AST_Expression v = generate_condition(c);
	return ADD( MULT( v , eq1 )  ,  MULT ( SUB(I(1), v )   , eq2 )  );
} 


MMO_Statement MMO_Replace_Equation_::replace_when_eq (AST_Equation eq) 
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
					foreach(eqit, qelse->equations()) stmList->push_back( replace_when_eq(current(eqit)) );
					
					elseList->push_back( newAST_Statement_Else( qelse->condition() , stmList   ) );
				}	
			}
			
			AST_StatementList stmList = newAST_StatementList();
			AST_EquationListIterator eqit;
			foreach(eqit, when->equationList()) stmList->push_back( replace_when_eq(current(eqit)) );
			
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
					foreach(eqit, qelse->equations()) stmList->push_back( replace_when_eq(current(eqit)) );
					
					elseList->push_back( newAST_Statement_Else( qelse->condition() , stmList   ) );
				}	
			}
			
			AST_StatementList stmList = newAST_StatementList();
			AST_EquationListIterator eqit;
			foreach(eqit, i->equationList()) stmList->push_back( replace_when_eq(current(eqit)) );
			
			AST_StatementList stmElseList = newAST_StatementList();
			foreach(eqit, i->equationElseList()) stmElseList->push_back( replace_when_eq(current(eqit)) );
			
			return newAST_Statement_If( i->condition() , stmList , elseList,stmElseList); 
			
		}
	
		case EQFOR:
		{
			AST_Equation_For f = eq->getAsFor();
			AST_StatementList stmList = newAST_StatementList();
			AST_EquationListIterator eqit;
			foreach(eqit, f->equationList()) stmList->push_back( replace_when_eq(current(eqit)) );
			return newAST_Statement_For(f->forIndexList() , stmList );
		}
		
	}	
}

