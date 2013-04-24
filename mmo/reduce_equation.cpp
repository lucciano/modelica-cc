
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

#include <mmo/reduce_equation.h>

#include <iostream>
using namespace std;

MMO_Reduce_Equation_::MMO_Reduce_Equation_ (MMO_Class * c): _c(c) {} ;


AST_Equation MMO_Reduce_Equation_::simplify( AST_Equation e) {
	AST_Expression ex;
	switch (e->equationType()) {
		case EQEQUALITY:
		{
			AST_Equation_Equality _e =  e->getAsEquality();
			Type t = _c->getExpresionType(  _e->right());
			
			// Just for Now
			switch (t->getType()) {
				case TYINTEGER:
				case TYREAL:
					ex = simplify_real(  _e->right() );
					break;
				case TYSTRING:
					return e;
					
				case TYBOOLEAN:
					ex = simplify_bool(  _e->right() , true);
					break;
				default:
					throw "This type is not suported";
			}
			return newAST_Equation_Equality( _e->left() , ex) ;
		}	
			
		default:
			throw "Not implemented yet!!";
	}
}

AST_Expression MMO_Reduce_Equation_::simplify_real(AST_Expression e)
{
	switch (e->expressionType()) {
		case EXPBINOP:
		  {
			AST_Expression_BinOp b = e->getAsBinOp();
			return newAST_Expression_BinOp(simplify_real(b->left()),simplify_real(b->right()), b->binopType());
			
		  }
		case EXPUMINUS: 
		  {
			AST_Expression_UMinus m = e->getAsUMinus();
			return newAST_Expression_UnaryMinus(simplify_real(m->exp()));
		  }
    
		case EXPOUTPUT :
		{
			AST_Expression_Output b = e->getAsOutput();
			AST_ExpressionList ls = new list < AST_Expression > ();
			ls->push_back(   simplify_real( b->getExpressionList()->front() )    )	;
			return newAST_Expression_OutputExpressions(ls);
		}  
      
		case EXPIF:
		  {   
			
			AST_Expression_If i = e->getAsIf();
			if ( ! i->elseif_list()->empty() ) {
					
				AST_ExpressionListReverseIterator it;
				AST_Expression iff;
				AST_ExpressionList iflist = i->elseif_list();
				
				foreachReverse(it,iflist) {
					if (current(it)->expressionType() == 8) {
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
			
			return newAST_Expression_If( simplify_bool(i->condition(),false) , simplify_real(i->then()), i->elseif_list(), simplify_real(i->else_exp()));
		  }
		  
		default:
			return e;
	}  

}

AST_String MMO_Reduce_Equation_::new_label()
{
	static int n = 0;
	stringstream ret;
	ret << "z_" << n ;
	return _S(ret.str());
}

bool simpleExpresion(AST_Expression e) {
	switch (e->expressionType())
	{
		case EXPBOOLEAN:
		case EXPSTRING: 
		case EXPREAL:
		case EXPINTEGER:
		case EXPCOMPREF:
			return true;
		default:
			return false;
	}
}

AST_Expression MMO_Reduce_Equation_::simplify_bool(AST_Expression e, bool flag)
{
		switch (e->expressionType()) {
		case EXPBINOP:
		  {
			AST_Expression_BinOp b = e->getAsBinOp();
			switch (b->binopType()) {
				case BINOPLOWER: 
				case BINOPLOWEREQ: 
				case BINOPGREATER: 
				case BINOPGREATEREQ: 
				case BINOPCOMPNE: 
				case BINOPCOMPEQ: 
				{
					
					if ( simpleExpresion(b->left()) and simpleExpresion(b->right()) and flag )
						return e;
						
					AST_String name = _S(new_label());
					_c->addVariable( name ,_S("Boolean"));	
					_c->addEquation( newAST_Equation_Equality( newAST_Expression_ComponentReferenceExp(name),b)  );
					
					return newAST_Expression_ComponentReferenceExp(name);

				}				
				default:
					return newAST_Expression_BinOp(simplify_bool(b->left(),flag),simplify_bool(b->right(),flag), b->binopType());
				
			}
			
			
		  }
		case EXPUMINUS: 
		  {
			AST_Expression_UMinus m = e->getAsUMinus();
			return newAST_Expression_UnaryMinus(simplify_bool(m->exp() , flag));
		  }
    
		case EXPOUTPUT :
		{
			AST_Expression_Output b = e->getAsOutput();
			AST_ExpressionList ls = new list < AST_Expression > ();
			ls->push_back(   simplify_bool( b->getExpressionList()->front() ,flag)    )	;
			return newAST_Expression_OutputExpressions(ls);
		}  
      
		case EXPIF:
		  {   
			
			AST_Expression_If i = e->getAsIf();
			if ( ! i->elseif_list()->empty() ) {
					
				AST_ExpressionListReverseIterator it;
				AST_Expression iff;
				AST_ExpressionList iflist = i->elseif_list();
				
				foreachReverse(it,iflist) {
					if (current(it)->expressionType() == 8) {
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
			
			return newAST_Expression_If( simplify_bool(i->condition(),flag) , simplify_real(i->then()), i->elseif_list(), simplify_real(i->else_exp()));
		  }
		  
		default:
			return e;
	}  
}


