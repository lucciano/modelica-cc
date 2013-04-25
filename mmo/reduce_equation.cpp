
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
					ex = simplify_real(_e->right());
					break;
				case TYSTRING:
					return e;
					
				case TYBOOLEAN:
					ex = simplify_bool(_e->right());
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
			
			return newAST_Expression_If( simplify_condition(i->condition()) , simplify_real(i->then()), i->elseif_list(), simplify_real(i->else_exp()));
		  }
		  
		default:
			return e;
	}  

}

AST_String MMO_Reduce_Equation_::new_label()
{
	static int n = 0 ;
	stringstream ret;
	ret << "z_" << n ;
	n++;
	return _S(ret.str());
}

bool simpleExpresion(AST_Expression e) {
	switch (e->expressionType())
	{
		case EXPOUTPUT :
		{
			AST_Expression_Output b = e->getAsOutput();
			return simpleExpresion( b->getExpressionList()->front()); 
		}  
		
		case EXPBINOP:
		{
				AST_Expression_BinOp b = e->getAsBinOp();
				switch (b->binopType()) {
					case BINOPDIV:
					case BINOPELDIV:
					case BINOPMULT:
					case BINOPELMULT:
					case BINOPADD:
					case BINOPELADD:
					case BINOPSUB:
					case BINOPELSUB:
					case BINOPEXP:
					case BINOPELEXP:
					{
						return simpleExpresion(b->left() ) && simpleExpresion(b->right() );
					}
					case BINOPAND:
					case BINOPOR:
					{
						return simpleExpresion(b->left() ) && simpleExpresion(b->right() );
					}	
					default: 
						return false;
					
				}
		}		
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

AST_Expression MMO_Reduce_Equation_::simplify_condition(AST_Expression e)
{
	
	switch (e->expressionType()) {
		case EXPBINOP:
		{
			AST_Expression_BinOp b = e->getAsBinOp();
			switch (b->binopType()) {
				case BINOPAND:
				case BINOPOR:
						return newAST_Expression_BinOp(simplify_condition(b->left()),simplify_condition(b->right()), b->binopType());
			}
			break;
		}
		
		case EXPCOMPREF:
		{
			return e;
		}
		
		case EXPOUTPUT :
		{
			AST_Expression_Output b = e->getAsOutput();
			AST_ExpressionList ls = new list < AST_Expression > ();
			ls->push_back(   simplify_condition( b->getExpressionList()->front() )    )	;
			return newAST_Expression_OutputExpressions(ls);
		}  
		
	}
	
	AST_String name = _S(new_label());
	_c->addVariable( name ,_S("Boolean"));	
	_c->addEquation( newAST_Equation_Equality( newAST_Expression_ComponentReferenceExp(name),e)  );
	
	return newAST_Expression_ComponentReferenceExp(name);
	
}



AST_Expression MMO_Reduce_Equation_::simplify_bool(AST_Expression e)
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
				case BINOPAND:
				case BINOPOR:
				{
					bool bl = simpleExpresion(b->left());
					bool br = simpleExpresion(b->right());
					bool logic =  b->binopType() == BINOPAND || b->binopType() == BINOPOR ;
					AST_Expression n1;
					AST_Expression n2;
					if ( bl and br)	return e;
					if (!bl) {	
						AST_String name = _S(new_label());
						_c->addVariable( name , logic ? _S("Boolean") : _S("Real"));	
						_c->addEquation( newAST_Equation_Equality( newAST_Expression_ComponentReferenceExp(name), b->left() )  );
						 n1 =  newAST_Expression_ComponentReferenceExp(name);
					} else 
						n1 = b->left();
						
					if (!br) {	
						AST_String name = _S(new_label());
						_c->addVariable( name , logic ? _S("Boolean") : _S("Real"));	
						_c->addEquation( newAST_Equation_Equality( newAST_Expression_ComponentReferenceExp(name), b->right() )  );
						 n2 =  newAST_Expression_ComponentReferenceExp(name);
					} else 
						n2 = b->right();	


					return newAST_Expression_BinOp(n1,n2, b->binopType());
				}
								
				default: // ?
					return newAST_Expression_BinOp(b->left(),b->right(), b->binopType());
				
			}
			
			
		  }
		case EXPUMINUS: 
		  {
			AST_Expression_UMinus m = e->getAsUMinus();
			return newAST_Expression_UnaryMinus(simplify_bool(m->exp()));
		  }
    
		case EXPOUTPUT :
		{
			AST_Expression_Output b = e->getAsOutput();
			AST_ExpressionList ls = new list < AST_Expression > ();
			ls->push_back(   simplify_bool( b->getExpressionList()->front())    )	;
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
			

			AST_Expression n1;
			AST_Expression n2;

			if ( i->then()->expressionType() != EXPCOMPREF) {	
				AST_String name = _S(new_label());
				_c->addVariable( name , _S("Boolean"));	
				_c->addEquation( newAST_Equation_Equality( newAST_Expression_ComponentReferenceExp(name), i->then() )  );
				 n1 =  newAST_Expression_ComponentReferenceExp(name);
			} else 
				n1 = i->then();
				
			if (( i->else_exp()->expressionType() != EXPCOMPREF)) {	
				AST_String name = _S(new_label());
				_c->addVariable( name , _S("Boolean") );	
				_c->addEquation( newAST_Equation_Equality( newAST_Expression_ComponentReferenceExp(name), i->else_exp() )  );
				 n2 =  newAST_Expression_ComponentReferenceExp(name);
			} else 
				n2 = i->else_exp();
						
			
			return newAST_Expression_If( simplify_condition(i->condition()) ,n1, i->elseif_list(), n2);
		  }
		  
		default:
			return e;
	}  
}


