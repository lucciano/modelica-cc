
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

MMO_ToMicroModelica_::MMO_ToMicroModelica_ (MMO_Class c): _c(c) 
{
	_pre = new std::set<string>;
} ;


void MMO_ToMicroModelica_::transform()
{
	/* Aca comienza todo el ciclo */
	checkStatement(_c->getStatements());
	checkStatement(_c->getIniStatements());
	
	initialFrame.push(_c->getIniStatements());
	
	
	transformEqList(_c->getIniEquations() , _c->getIniStatements(), NULL );
	transformEqList(_c->getEquations()    , _c->getStatements()   , NULL );
	
	/* Cambiamos los tipos y constantes Booleanas */ 
	for(int i = 0; i < _c->getVarSymbolTable()->count();i++) {
		VarInfo  v = _c->getVarSymbolTable()->varInfo(i);
		string n   = _c->getVarSymbolTable()->varName(i);
		v->setComment(NULL);
		v->setModification( ChangeModifications(v->modification()));
		if (_pre->find(n) != _pre->end())
			v->setDiscrete();
		
		if ( ( !v->isConstant() && !v->isParameter() ) || v->type()->getType() == TYBOOLEAN) v->setType(ChangeToReal(v->type()));
	}
	ChangePre(_c->getIniEquations());
	ChangePre(_c->getEquations());
}


void MMO_ToMicroModelica_::transformEqList(AST_EquationList eqList , AST_StatementList stList, IndexMap iMap)
{
	AST_EquationListIterator eqit;
	list<AST_EquationListIterator> * del = new list<AST_EquationListIterator>();
	
    foreach(eqit,eqList) {		
		AST_Equation eq = current_element(eqit);

		switch (eq->equationType()) 
		{
			case EQEQUALITY:
			{
				AST_Equation _e = toMicro_eq_equality(eq->getAsEquality(), stList , iMap);
				
				if (_e != NULL)  current_element(eqit) = _e;
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
				toMicro_eq_when(when,stList);
				AST_ListAppend(del,eqit);
				break;
			}
			
			case EQIF:
			{
				AST_EquationList ls = toMicro_eq_if(eq->getAsIf(),stList ,iMap);
				AST_EquationListIterator eqxit;
				foreach(eqxit,ls)  AST_ListAppend(eqList,current_element(eqxit));
				AST_ListAppend(del,eqit);
				break;
			}	
			
			case EQFOR:
			{
				AST_Equation _e = toMicro_eq_for(eq->getAsFor(),stList,iMap);
				current_element(eqit) = _e;
				break;
			}
			
			case EQCALL:
			{			
				AST_Expression_Call c = eq->getAsCall()->call()->getAsCall();
				if (toStr(c->name()) == "assert" ) 
					AST_ListAppend(del,eqit);
				break;
			}	
			
			default:
				cerr << "Error: "  << eq->equationType() << endl;
				throw "Not implemented yet!!";
			
		}		
	}
	
	list<AST_EquationListIterator>::iterator delIt;
	foreach(delIt,del) eqList->erase(current_element(delIt));	
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
			return newAST_Expression_BinOp(b->left(),b->right(), BINOPGREATEREQ  );
		case BINOPLOWEREQ: 
			return newAST_Expression_BinOp(b->left(),b->right(), BINOPGREATER );
		case BINOPGREATER: 
			return newAST_Expression_BinOp(b->left(),b->right(), BINOPLOWEREQ );
		case BINOPGREATEREQ: 
			return newAST_Expression_BinOp(b->left(),b->right(), BINOPLOWER  );
		case BINOPCOMPNE: 
			return newAST_Expression_BinOp(b->left(),b->right(), BINOPCOMPEQ );
		case BINOPCOMPEQ: 
			return newAST_Expression_BinOp(b->left(),b->right(), BINOPCOMPNE );
		default:
			throw "No deberia pasar";
	}
}

MMO_Statement MMO_ToMicroModelica_::make_if(AST_Expression cond , AST_Expression_ComponentReference var)
{
	AST_StatementList l1          = newAST_SimpleList( newAST_Statement_Assign(var , I(1)) );
	AST_StatementList l2          = newAST_SimpleList( newAST_Statement_Assign(var , I(0)) );
	AST_Statement_ElseList elList = newAST_Statement_ElseList(); 
	return newAST_Statement_If( cond , l1 , elList , l2 );	
}

MMO_Statement MMO_ToMicroModelica_::make_when(AST_Expression cond , AST_Expression_ComponentReference var)
{
	AST_StatementList l1          = newAST_SimpleList( newAST_Statement_Assign(var , I(1)) );
	AST_StatementList l2          = newAST_SimpleList( newAST_Statement_Assign(var , I(0)) );
	AST_Statement_ElseList elList = newAST_SimpleList( newAST_Statement_Else ( negar_cond(cond) , l2 )); 
	return newAST_Statement_When( cond , l1 , elList );	
}


MMO_Statement MMO_ToMicroModelica_::make_ifABS(AST_Expression e , AST_Expression_ComponentReference var)
{
	AST_StatementList l1          = newAST_SimpleList( newAST_Statement_Assign(var , e) );
	AST_StatementList l2          = newAST_SimpleList( newAST_Statement_Assign(var , UNARYM(e)) );
	AST_Statement_ElseList elList = newAST_Statement_ElseList(); 
	return newAST_Statement_If( GREATER(e,I(0)) , l1 , elList , l2 );	
}

MMO_Statement MMO_ToMicroModelica_::make_whenABS(AST_Expression e , AST_Expression_ComponentReference var)
{
	AST_StatementList l1          = newAST_SimpleList( newAST_Statement_Assign(var , e) );
	AST_StatementList l2          = newAST_SimpleList( newAST_Statement_Assign(var , UNARYM(e)) );
	AST_Statement_ElseList elList = newAST_SimpleList( newAST_Statement_Else ( LOWER(e,I(0)) , l2 )); 
	return newAST_Statement_When( GREATER(e,I(0)) , l1 , elList );	
}

MMO_Statement MMO_ToMicroModelica_::make_ifMin(AST_Expression e1 , AST_Expression e2 , AST_Expression_ComponentReference var)
{
	AST_StatementList l1          = newAST_SimpleList( newAST_Statement_Assign(var , e1) );
	AST_StatementList l2          = newAST_SimpleList( newAST_Statement_Assign(var , e2) );
	AST_Statement_ElseList elList = newAST_Statement_ElseList(); 
	return newAST_Statement_If( LOWER(e1,e2) , l1 , elList , l2 );	
}

MMO_Statement MMO_ToMicroModelica_::make_whenMin(AST_Expression e1,AST_Expression e2 , AST_Expression_ComponentReference var)
{
	AST_StatementList l1          = newAST_SimpleList( newAST_Statement_Assign(var , e1) );
	AST_StatementList l2          = newAST_SimpleList( newAST_Statement_Assign(var , e2) );
	AST_Statement_ElseList elList = newAST_SimpleList( newAST_Statement_Else ( GREATER(e1,e2) , l2 )); 
	return newAST_Statement_When( LOWER(e1,e2) , l1 , elList );	
}

MMO_Statement MMO_ToMicroModelica_::make_ifMax(AST_Expression e1 , AST_Expression e2 , AST_Expression_ComponentReference var)
{
	AST_StatementList l1          = newAST_SimpleList( newAST_Statement_Assign(var , e1) );
	AST_StatementList l2          = newAST_SimpleList( newAST_Statement_Assign(var , e2) );
	AST_Statement_ElseList elList = newAST_Statement_ElseList(); 
	return newAST_Statement_If( GREATER(e1,e2) , l1 , elList , l2 );	
}

MMO_Statement MMO_ToMicroModelica_::make_whenMax(AST_Expression e1,AST_Expression e2 , AST_Expression_ComponentReference var)
{
	AST_StatementList l1          = newAST_SimpleList( newAST_Statement_Assign(var , e1) );
	AST_StatementList l2          = newAST_SimpleList( newAST_Statement_Assign(var , e2) );
	AST_Statement_ElseList elList = newAST_SimpleList( newAST_Statement_Else ( LOWER(e1,e2) , l2 )); 
	return newAST_Statement_When( GREATER(e1,e2) , l1 , elList );	
}

MMO_Statement MMO_ToMicroModelica_::make_ifSING(AST_Expression e , AST_Expression_ComponentReference var)
{
	AST_StatementList l1          = newAST_SimpleList( newAST_Statement_Assign(var , I( 1)) );
	AST_StatementList l2          = newAST_SimpleList( newAST_Statement_Assign(var , I( 0)) );
	AST_StatementList l3          = newAST_SimpleList( newAST_Statement_Assign(var , I(-1)) );
	AST_Statement_ElseList elList = newAST_Statement_ElseList();
	AST_Statement_ElseList elList2 = newAST_Statement_ElseList(); 
	AST_StatementList ll		  = newAST_SimpleList( newAST_Statement_If( EQUAL(e,I(0)) , l2 , elList , l1 ) );
	return newAST_Statement_If( GREATEREQ(e,I(0)) , ll , elList2 , l3 );	
}

MMO_Statement MMO_ToMicroModelica_::make_whenSING(AST_Expression e , AST_Expression_ComponentReference var)
{
	AST_StatementList l1          = newAST_SimpleList( newAST_Statement_Assign(var , I( 1)) );
	AST_StatementList l2          = newAST_SimpleList( newAST_Statement_Assign(var , I( 0)) );
	AST_StatementList l3          = newAST_SimpleList( newAST_Statement_Assign(var , I(-1)) );
	AST_Statement_ElseList elList2 = newAST_Statement_ElseList(); 
	AST_StatementList ll		  = newAST_SimpleList( newAST_Statement_If( EQUAL(e,I(0)) , l2 , elList2 , l1 ) );
	AST_Statement_ElseList elList = newAST_SimpleList( newAST_Statement_Else ( LOWER(e,I(0)) , l3 )); 
	return newAST_Statement_When( GREATEREQ(e,I(0)) , ll , elList );	
}


MMO_Equation MMO_ToMicroModelica_::toMicro_eq_equality(AST_Equation_Equality eq , AST_StatementList stList,IndexMap iMap)
{	
	AST_Expression r = eq->right();
	AST_Expression l = eq->left();
	
	
	try {	
		if (IS_CREF(l) &&  _c->getExpresionType(r)->getType() == TYBOOLEAN ) {		
			AST_Expression_ComponentReference cr = l->getAsComponentReference();		
			FindReference fr(_S(cr->name()));
			if (fr.foldTraverse(r)){
				AST_Expression_ComponentReference c2 = create_Variable()->getAsComponentReference();
				ReplaceReference rr ( _S(cr->name()) , _S(c2->name()) );
				AST_Expression _r = rr.foldTraverse(toMicro_exp(r,stList,iMap));	
				AST_Expression cond = GREATER(_r , _R(0.5));
				AST_ListAppend(stList, make_when( cond  ,  c2 ) );
				AST_ListAppend(initialFrame.top(), make_if( cond , c2 ));
				return newAST_Equation_Equality( l , _r );	
			}	
		}
		
		if (IS_CREF(r) &&  _c->getExpresionType(l)->getType() == TYBOOLEAN ) {		
			AST_Expression_ComponentReference cr = r->getAsComponentReference();		
			FindReference fr(_S(cr->name()));
			if (fr.foldTraverse(l)){
				AST_Expression_ComponentReference c2 = create_Variable()->getAsComponentReference();
				ReplaceReference rr ( _S(cr->name()) , _S(c2->name()) );
				AST_Expression _l = rr.foldTraverse(toMicro_exp(l,stList,iMap));	
				AST_Expression cond = GREATER(_l , _R(0.5));
				AST_ListAppend(stList, make_when( cond  ,  c2 ) );
				AST_ListAppend(initialFrame.top(), make_if( cond, c2 ));
				return newAST_Equation_Equality( _l , r );	
			}	
		}
		
	} catch (...) {} 
	
	AST_Expression _r = toMicro_exp(r,stList,iMap);
	AST_Expression _l = toMicro_exp(l,stList,iMap);
	
	return newAST_Equation_Equality( _l , _r );
}

AST_Expression MMO_ToMicroModelica_::create_Variable()
{
	AST_String name = new_label();
	_c->addVariable( name , _S("Real"),  newAST_TypePrefix(TP_DISCRETE));	
	return newAST_Expression_ComponentReferenceExp (name);	
}

AST_Expression MMO_ToMicroModelica_::toMicro_exp(AST_Expression e , AST_StatementList stList , IndexMap iMap)
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
					AST_Expression e1 =  toMicro_exp(b->left(),stList,iMap);
					AST_Expression e2 =  toMicro_exp(b->right(),stList,iMap);
					AST_ExpressionList indexList = newAST_ExpressionList();
					AST_ExpressionList sList = newAST_ExpressionList();
					if (iMap) {
						IndexMap_::iterator it;
						foreach(it,iMap) 
							if (IndexAccess(e1,it->first) || IndexAccess(e2,it->first)  ) {
								AST_ListAppend(sList,  newAST_Expression_ComponentReferenceExp( _S(it->first) ));
								AST_ListAppend(indexList,it->second);
							}
					}
					
					AST_String name = new_label();
					AST_Expression cr;
					if (sList->size() == 0 ) {
						_c->addVariable( name , _S("Real"),  newAST_TypePrefix(TP_DISCRETE));	
						cr = newAST_Expression_ComponentReferenceExp (name)->getAsComponentReference();	
					}else{ 
						_c->addVariable( name , _S("Real"),newAST_TypePrefix(TP_DISCRETE), indexList);	
						cr = AST_Expression_ComponentReference_Add(newAST_Expression_ComponentReference(),name,sList)->getAsComponentReference();
					}
					AST_Expression _b = newAST_Expression_BinOp( e1 , e2 ,b->binopType()  );
					AST_ListAppend(stList, make_when( _b, cr->getAsComponentReference() ));
					AST_ListAppend(initialFrame.top(), make_if( _b, cr->getAsComponentReference() ));
					return newAST_Expression_Call(_S("pre"), NULL , newAST_SimpleList(cr));
					
				}
				
				case BINOPAND:
				{
					return MULT( toMicro_exp(b->left(),stList,iMap) , toMicro_exp(b->right(),stList,iMap) );
				}
				
				case BINOPOR:
				{
					return SUB(I(1) , MULT( SUB(I(1) , toMicro_exp(b->left(),stList,iMap)) , SUB(I(1) ,toMicro_exp(b->right(),stList,iMap) ) ) ) ;
				}	
								
				default:
				{
					return newAST_Expression_BinOp( toMicro_exp(b->left(),stList,iMap) , toMicro_exp(b->right(),stList,iMap) ,b->binopType()  );
				}
				 
			}
		}
		
		case EXPUMINUS:
		{
			AST_Expression_UMinus u = e->getAsUMinus();
			return newAST_Expression_UnaryMinus( toMicro_exp(u->exp(),stList,iMap) );
		}
		
		case EXPOUTPUT :
		{
			AST_Expression_Output b = e->getAsOutput();
			return newAST_Expression_OutputExpressions(newAST_SimpleList(toMicro_exp(b->expressionList()->front(),stList ,iMap)));
		}
		
		case EXPCALL:
		{
			AST_Expression_Call call = e->getAsCall();
			if (toStr(call->name())  == "edge"){ 
				return toMicro_exp( GREATER(  call->arguments()->front()   , _R(0.5) ) , stList , iMap) ;
			}
			if (toStr(call->name())  == "abs"){
				AST_Expression cr = create_Variable();
				AST_Expression e = toMicro_exp(call->arguments()->front() , stList , iMap);
				AST_ListAppend(stList, make_whenABS(e, cr->getAsComponentReference()));	
				AST_ListAppend(initialFrame.top(), make_ifABS( e, cr->getAsComponentReference() ));
				return cr;
			}
			if (toStr(call->name())  == "sign"){
				AST_Expression cr = create_Variable();
				AST_Expression e = toMicro_exp(call->arguments()->front() , stList , iMap);
				AST_ListAppend(stList, make_whenSING(e, cr->getAsComponentReference()));	
				AST_ListAppend(initialFrame.top(), make_ifSING( e, cr->getAsComponentReference() ));
				return cr;
			}
			if (toStr(call->name())  == "min"){
				AST_Expression cr = create_Variable();
				AST_Expression e1 = toMicro_exp(call->arguments()->front() , stList , iMap);
				AST_Expression e2 = toMicro_exp(current_element(++call->arguments()->begin()) , stList , iMap);
				AST_ListAppend(stList, make_whenMin(e1,e2, cr->getAsComponentReference()));	
				AST_ListAppend(initialFrame.top(), make_ifMin( e1,e2, cr->getAsComponentReference() ));
				return cr;
			}
			if (toStr(call->name())  == "max"){
				AST_Expression cr = create_Variable();
				AST_Expression e1 = toMicro_exp(call->arguments()->front() , stList , iMap);
				AST_Expression e2 = toMicro_exp(current_element(++call->arguments()->begin()) , stList , iMap);
				AST_ListAppend(stList, make_whenMax(e1,e2, cr->getAsComponentReference()));	
				AST_ListAppend(initialFrame.top(), make_ifMax( e1,e2, cr->getAsComponentReference() ));
				return cr;
			}
			if (toStr(call->name())  == "noevent") 
				return call;
			
			AST_ExpressionListIterator it;
			foreach(it , call->arguments()) current_element(it) = toMicro_exp(current_element(it) , stList , iMap);	
			return call;
		}
		
		case EXPCOMPREF:
		{
			return e;
		}
			
			
		case EXPBOOLEANNOT:	
		{
			AST_Expression_BooleanNot no = e->getAsBooleanNot();
			return SUB(I(1) , toMicro_exp( no->exp() ,stList ,iMap) ) ;
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
					if (current_element(it)->expressionType() == EXPELSEIF) {
						AST_Expression_If_ElseIf elif = current_element(it)->getAsElseIf();
						if  (it == iflist->rbegin())
							iff = newAST_Expression_If( elif->condition() , elif->then() , newAST_ExpressionList() ,i->else_exp() );					
						else 	
							iff = newAST_Expression_If( elif->condition() , elif->then() , newAST_ExpressionList() , iff );
					}
				}
				iff = newAST_Expression_If( i->condition() , i->then() , newAST_ExpressionList() , iff );
				i = iff->getAsIf();
			}
			
			AST_Expression eq1 = toMicro_exp(i->then(),stList,iMap);
			AST_Expression eq2 = toMicro_exp(i->else_exp(),stList,iMap);
			AST_Expression cond = toMicro_exp(i->condition(),stList,iMap);
			return ADD( MULT( cond , eq1 )  ,  MULT ( SUB(I(1), cond )   , eq2 )  );
			
		}	
			
		default:
			return e;
		
	}
	
}


AST_Expression MMO_ToMicroModelica_::makeCondition(AST_ExpressionList ls , unsigned int n )
{
	AST_ExpressionListIterator exit =  ls->begin();
	unsigned int i;
	if (n==0) return  current_element(exit);
	AST_Expression e = UMENOS(current_element(exit)) ;
	exit++;
	for(i = 1; i < n; i++ ){
		e = MULT( e , UMENOS(  current_element(exit) ));
		exit++;
	}
	if (n < ls->size())
		e = MULT( e , current_element(exit));
	return e;
	
}

/*AST_Equation  MMO_ToMicroModelica_::makeEquation(AST_EquationList ls , AST_ExpressionList cond,MMO_StatementList stList,IndexMap iMap)
{
	unsigned int i = 0;
	AST_Expression e1,e2;
	AST_EquationListIterator eqit = ls->begin();
	e1 = MULT( toMicro_exp(current_element(eqit)->getAsEquality()->left(),stList,iMap)   , makeCondition(cond,0) );
	e2 = MULT( toMicro_exp(current_element(eqit)->getAsEquality()->right(),stList,iMap)  , makeCondition(cond,0) );
	eqit++;
	for (i = 1; i < ls->size(); i ++ ){
		e1 = ADD(e1,MULT( toMicro_exp(current_element(eqit)->getAsEquality()->left(),stList,iMap)   , makeCondition(cond,i) ));
		e2 = ADD(e2,MULT( toMicro_exp(current_element(eqit)->getAsEquality()->right(),stList,iMap)  , makeCondition(cond,i) ));
		eqit++;
	}
	return newAST_Equation_Equality(e1,e2);
}*/

AST_Equation  MMO_ToMicroModelica_::makeEquation(AST_EquationList ls , AST_ExpressionList cond,MMO_StatementList stList,IndexMap iMap)
{
	unsigned int i = 0;
	AST_Expression e1,e2;
	AST_EquationListIterator eqit = ls->begin();
	e1 = MULT( current_element(eqit)->getAsEquality()->left() , makeCondition(cond,0) );
	e2 = MULT( current_element(eqit)->getAsEquality()->right() , makeCondition(cond,0) );
	eqit++;
	for (i = 1; i < ls->size(); i ++ ){
		e1 = ADD(e1,MULT( current_element(eqit)->getAsEquality()->left()   , makeCondition(cond,i) ));
		e2 = ADD(e2,MULT( current_element(eqit)->getAsEquality()->right()  , makeCondition(cond,i) ));
		eqit++;
	}
	return newAST_Equation_Equality(e1,e2);
}

AST_EquationList MMO_ToMicroModelica_::toMicro_eq_if(AST_Equation_If iff , MMO_StatementList stList,IndexMap iMap)
{
	AST_ExpressionList condList = newAST_ExpressionList();
	AST_ListAppend(condList,toMicro_exp(iff->condition(),stList,iMap));
	
	/*  Lista de condiciones */ 
	if ( ! iff->equationElseIf()->empty() ) {
		AST_Equation_ElseListIterator elseit;
		foreach(elseit, iff->equationElseIf()) AST_ListAppend(condList, toMicro_exp(current_element(elseit)->condition(),stList,iMap) );
	}
	int nro =  iff->equationList()->size();
	
	/*  Re ordeno las ecuaciones */ 
	AST_EquationList * ls = new AST_EquationList[nro];
	for(int i = 0; i < nro;i++) ls[i] = newAST_EquationList();
	
	AST_EquationListIterator eqit;
	int k = 0;

	transformEqList(iff->equationList() , stList , iMap);
	
	foreach (eqit, iff->equationList()) AST_ListAppend( ls[k++] , current_element(eqit) );
		
	if ( ! iff->equationElseIf()->empty() ) {
		AST_Equation_ElseListIterator elseit;
		foreach(elseit, iff->equationElseIf()) {
			AST_Equation_Else elseIf =  current_element(elseit);
			k = 0;
			transformEqList(elseIf->equations() , stList , iMap);
			foreach( eqit , elseIf->equations() ) AST_ListAppend( ls[k++] , current_element(eqit) );
		}
	}
	k=0;
	transformEqList(iff->equationElseList() , stList , iMap);
	foreach (eqit, iff->equationElseList()) AST_ListAppend( ls[k++] , current_element(eqit) );
	
	/* Ahora transformo y creo */
	AST_EquationList eqList = newAST_EquationList();
	for(k= 0;k < nro;k++) AST_ListAppend(eqList,makeEquation(ls[k],condList,stList,iMap));
	return eqList;
}
 
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
			return newAST_Expression_OutputExpressions(newAST_SimpleList(whenCondition(b->expressionList()->front() , ls )));
		}
		
		case EXPCOMPREF:
		{
			AST_Expression_ComponentReference cf = e->getAsComponentReference();
			Type t = _c->getVariableType(cf->names()->front());
			if (t != NULL && t->getType() == TYBOOLEAN) {
				return GREATER( cf , _R(0.5)  );
			} else 
				return cf;
		}
		
		case EXPCALL:
		{
			AST_Expression_Call call = e->getAsCall();
			if (  toStr(call->name()) == "sample") {
				AST_String name = new_label();   // TNEXT
				_c->addVariable( name , _S("Real") , newAST_TypePrefix(TP_DISCRETE) , current_element(call->arguments()->begin()) );
				AST_Expression_ComponentReference cr = newAST_Expression_ComponentReferenceExp (name)->getAsComponentReference();
				
				AST_Expression per =   current_element (  ++call->arguments()->begin()   );
				
				AST_ListAppend(ls ,  newAST_Statement_Assign(cr, ADD(  cr ,per )  ) );
				return GREATER( _VAR(_S("time")) , cr );
			}
			
			if ( toStr(call->name()) == "initial") {
				return GREATER( _VAR(_S("time")) , I(0) );
			}
			
			if (toStr(call->name())  == "edge" ) {
				return GREATER(  call->arguments()->front()   , _R(0.5) );
			}
			
			return call;
		}
		default:
			return e;
	}	
} 
 

void MMO_ToMicroModelica_::toMicro_eq_when (AST_Equation eq, MMO_StatementList stms) 
{
	
	switch (eq->equationType()) {
		case EQEQUALITY:
		{ 
			AST_Equation_Equality _e =  eq->getAsEquality();
			WhenEqualityTrasforms rb;
			AST_Expression _r = rb.foldTraverse(_e->right());
			if (_e->left()->expressionType() == EXPCOMPREF)
			{
				AST_Expression_ComponentReference cf = _e->left()->getAsComponentReference(); 
				_pre->insert((cf->name()));
				VarInfo varInfo = _c->getVarSymbolTable()->lookup( toStr(cf->names()->front()) );
				if (varInfo == NULL) throw "Variable no encontrada" ;
				if (varInfo->isState()) {
					AST_ExpressionList ls = newAST_ExpressionList(); 
					AST_ListAppend(ls,static_cast<AST_Expression>(cf));
					AST_ListAppend(ls,  _r );
					AST_Expression_ComponentReference aux = newAST_Expression_ComponentReferenceExp(_S("reinit"))->getAsComponentReference();
					AST_ListAppend(stms,newAST_Statement_Assign( aux , newAST_Expression_FunctionCallArgs(ls) ));
					return ;
				} else 
					AST_ListAppend(stms,newAST_Statement_Assign( cf  , _r ));
					return ;
			}
		}
	
		case EQWHEN:
		{
			AST_Equation_When when = eq->getAsWhen();
			AST_Statement_ElseList elseList = newAST_Statement_ElseList();
			if (when->equationElseWhen()->size() > 0 ) {
				AST_Equation_ElseListIterator elit;
				foreach(elit, when->equationElseWhen()){
					AST_Equation_Else qelse = current_element(elit);
					
					AST_StatementList stmList = newAST_StatementList();
					AST_EquationListIterator eqit;
					foreach(eqit, qelse->equations()) toMicro_eq_when(current_element(eqit),stmList) ;
					
					AST_Expression _cond = whenCondition(qelse->condition(), stmList);
					AST_ListAppend(elseList,newAST_Statement_Else( _cond , stmList   ) );
				}	
			}
			AST_StatementList stmList = newAST_StatementList();
			AST_EquationListIterator eqit;
			foreach(eqit, when->equationList()) toMicro_eq_when(current_element(eqit),stmList) ;
			
			if (when->condition()->expressionType() == EXPBRACE) {
				AST_ExpressionList cs = when->condition()->getAsBrace()->arguments();
				AST_ExpressionListIterator csit;
				foreach(csit,cs)  {
					AST_StatementList ctmList =  AST_ListCopy(stmList);
					AST_Expression _cond = whenCondition( current_element(csit), ctmList);
					AST_ListAppend(stms,newAST_Statement_When( _cond , ctmList , elseList));
				}
				
			} else {	
				AST_Expression _cond = whenCondition(when->condition(), stmList);
				AST_ListAppend(stms,newAST_Statement_When( _cond , stmList , elseList));
			}	
			return ;  
		}
		
		case EQIF:
		{
			AST_Equation_If i = eq->getAsIf();
			AST_Statement_ElseList elseList = newAST_Statement_ElseList();
			if (i->equationElseIf()->size() > 0 ) {
				AST_Equation_ElseListIterator elit;
				foreach(elit, i->equationElseIf()){
					AST_Equation_Else qelse = current_element(elit);
					
					AST_StatementList stmList = newAST_StatementList();
					AST_EquationListIterator eqit;
					foreach(eqit, qelse->equations()) toMicro_eq_when(current_element(eqit),stmList) ;
					
					AST_ListAppend(elseList,newAST_Statement_Else( qelse->condition() , stmList   ) );
				}	
			}
			
			AST_StatementList stmList = newAST_StatementList();
			AST_EquationListIterator eqit;
			foreach(eqit, i->equationList()) toMicro_eq_when(current_element(eqit),stmList);
			
			AST_StatementList stmElseList = newAST_StatementList();
			foreach(eqit, i->equationElseList()) toMicro_eq_when(current_element(eqit),stmElseList);
			
			AST_ListAppend(stms,newAST_Statement_If( i->condition() , stmList , elseList,stmElseList));
			return; 
			
		}
	
		case EQFOR:
		{
			AST_Equation_For f = eq->getAsFor();
			AST_StatementList stmList = newAST_StatementList();
			AST_EquationListIterator eqit;
			foreach(eqit, f->equationList()) toMicro_eq_when(current_element(eqit),stmList);
			AST_ListAppend(stms,newAST_Statement_For(f->forIndexList() , stmList ));
			return;
		}
		
		case EQCALL:
		{			
			AST_Expression_Call c = eq->getAsCall()->call()->getAsCall();
			AST_Expression_ComponentReference aux = newAST_Expression_ComponentReferenceExp( copyAST_String(c->name()) )->getAsComponentReference();
			AST_ListAppend(stms,newAST_Statement_Assign( aux , newAST_Expression_FunctionCallArgs(c->arguments()) ));
			return;
		}	
		
		default:
			throw "No implementado aun (toMicro_eq_when) ";
	}	 
}


IndexMap MMO_ToMicroModelica_::viewIndex(IndexMap imap, AST_ForIndexList list)
{
	IndexMap i = new IndexMap_ ;
	AST_ForIndexListIterator forit;
	typename IndexMap_::iterator it;
	if (imap) {
		for(it = imap->begin() ;  it !=  imap->end(); it++)
			(*i)[ it->first ] = it->second;   
	}
	foreach(forit,list) {
		AST_Expression aux;
		// For Now, get the back
		if (current_element(forit)->in_exp()->expressionType() == EXPBRACE )
			aux = current_element(forit)->in_exp()->getAsBrace()-> arguments()->back();
		if (current_element(forit)->in_exp()->expressionType() == EXPRANGE )
			aux = current_element(forit)->in_exp()->getAsRange()-> expressionList()->back();
		
		current_element(i)[ toStr(current_element(forit)->variable()) ] = aux;


	}
	return i;
}


MMO_Equation MMO_ToMicroModelica_::toMicro_eq_for (AST_Equation_For f, MMO_StatementList stList,IndexMap iMap)
{
	IndexMap _iMap = viewIndex(iMap,f->forIndexList());
	MMO_StatementList stLs = newAST_StatementList();
	
	initialFrame.push(newAST_StatementList());
	
	transformEqList(f->equationList() , stLs , _iMap);
	
	if (stLs->size() > 0)
		AST_ListAppend(stList , newAST_Statement_For(f->forIndexList() , stLs) ) ;
	
	MMO_StatementList iniLS = initialFrame.top();
	initialFrame.pop();
	
	if (iniLS->size() > 0)
		AST_ListAppend(initialFrame.top() , newAST_Statement_For(f->forIndexList() , iniLS) ) ;	
		
	return f;
} 


bool MMO_ToMicroModelica_::IndexAccess(AST_Expression e, string i )
{
	switch (e->expressionType())
	{
		case EXPBINOP:
		{
			AST_Expression_BinOp b = e->getAsBinOp();
			return IndexAccess(b->left(),i) || IndexAccess(b->right(),i);
		}
		
		case EXPUMINUS:
		{
			AST_Expression_UMinus u = e->getAsUMinus();
			return  IndexAccess(u->exp(),i);
		}
		
		case EXPBOOLEANNOT:
		{
			AST_Expression_BooleanNot no = e->getAsBooleanNot();
			return IndexAccess( no->exp() ,i);
		}
		
		case EXPOUTPUT:
		{
			AST_Expression_Output b = e->getAsOutput();
			return IndexAccess(b->expressionList()->front() , i ) 	; 
		}
		
		case EXPCALL:
		{
			AST_Expression_Call call = e->getAsCall();
			AST_ExpressionListIterator exit;
			bool b = false;
			foreach(exit,call->arguments())  b = b || IndexAccess( current_element(exit) , i );
			return b;
		}
		case EXPCOMPREF:
		{
			AST_Expression_ComponentReference cr = e->getAsComponentReference();
			if ( cr->names()->front()->compare(i) == 0 ) return true;
			AST_ExpressionListIterator exit;
			bool b = false;
			foreach(exit,cr->indexes()->front())  b = b || IndexAccess( current_element(exit) , i );
			return b;
			
		}
		
		case EXPBOOLEAN:
		case EXPSTRING:
		case EXPREAL:
		case EXPINTEGER:
			return false;
		
		default:
			return true;
	}	
} 

Type MMO_ToMicroModelica_::ChangeToReal(Type m)
{
	switch(m->getType()) {
		case TYREAL:
		case TYBOOLEAN:
		case TYINTEGER:
			return _c->getTypeSymbolTable()->lookup("Real");
		case TYARRAY:
			return new Type_Array_( ChangeToReal(m->getAsArray()->arrayOf())  ,  m->getAsArray()->dimension() );
		default:
			return m;
	}
	
}


void MMO_ToMicroModelica_::checkStatement(MMO_StatementList ls)
{
	AST_StatementListIterator it;
	foreach(it,ls) {
		AST_Statement st = current_element(it);
		switch ( st->statementType() )
		{
			case STASSING:
			{
				AST_Statement_Assign asg = st->getAsAssign();
				ReplaceBoolean rb;
				current_element(it) =  newAST_Statement_Assign( asg->lhs() , rb.foldTraverse(asg->exp())  );
				break;
				
			}
			
			case STFOR:
			{
				AST_Statement_For f = st->getAsFor();
				checkStatement( f->statements() );
				break;
			}
			
			case STWHEN:
			{
				AST_Statement_When  w = st->getAsWhen();
				checkStatement( w->statements() );
				AST_Statement_ElseList elList = newAST_Statement_ElseList();
				if (w->else_when()->size() > 0 ) {
					AST_Statement_ElseListIterator elseIt;
					foreach ( elseIt , w->else_when() ){
						checkStatement( current_element(elseIt)->statements() );
						AST_ListAppend(elList , newAST_Statement_Else( whenCondition(current_element(elseIt)->condition(), current_element(elseIt)->statements()) , current_element(elseIt)->statements() )   );
					}
				}
				current_element(it) = newAST_Statement_When( whenCondition(w->condition(),w->statements()) , w->statements()  , elList  );
				break;
				
			}
			
			default:
				break;
		}
	}
}


AST_Modification MMO_ToMicroModelica_::ChangeModifications(AST_Modification m)
{
	if (!m) return m;
	
	ReplaceBoolean rb;
	switch(m->modificationType())
	{
		case MODEQUAL:
		{
			AST_Modification_Equal eq = m->getAsEqual();
			return newAST_Modification_Equal( rb.foldTraverse(eq->exp())  );
		}
		
		case MODASSIGN:
		{
			AST_Modification_Assign asig = m->getAsAssign();
			return newAST_Modification_Assign( rb.foldTraverse(asig->exp()));
		}
		
		case MODCLASS:
		{
			AST_Modification_Class cc = m->getAsClass();
			AST_Expression _e = (cc->exp()) ? rb.foldTraverse(cc->exp()) : NULL  ;
			AST_ArgumentList args = newAST_ArgumentList();
			if (cc->arguments()->size() > 0) {
				AST_ArgumentListIterator it;
				foreach(it, cc->arguments()) {
					AST_Argument_Modification mm = current_element(it)->getAsModification();
					if ( toStr(mm->name()) == "start")	{
            AST_Argument mod = newAST_Argument_Modification(mm->name() , ChangeModifications(mm->modification()) );
            if (mm->isFinal())
              mod->setFinal();
            if (mm->hasEach())
              mod->setEach();
            AST_ListAppend(args , mod);
          }
					else AST_ListAppend(args,static_cast<AST_Argument>(mm));
				}
			}
			return newAST_Modification_Class(args,_e);
		}
		case MODNONE:
			throw "Error! (MMO_ToMicroModelica_::ChangeModifications)";
	}
	return NULL;
}

void MMO_ToMicroModelica_::ChangePre(AST_EquationList eqList)
{
	PreChange pc(_pre);
	AST_EquationListIterator eqit;
    foreach(eqit,eqList) {		
		AST_Equation eq = current_element(eqit);

		switch (eq->equationType()) 
		{
			case EQEQUALITY:
			{
				AST_Equation_Equality e = eq->getAsEquality();
				e->setLeft(pc.foldTraverse( e->left() ));
				e->setRight(pc.foldTraverse( e->right() ));
				break;
			}
			
			case EQNONE:
			{	
				break;
			}
			
			case EQFOR:
			{
				AST_Equation_For f = eq->getAsFor();
				ChangePre(f->equationList());
				break;
			}
			
			case EQCALL:
			{			
				//AST_Expression_Call c = eq->getAsCall()->call()->getAsCall();
				break;
			}	
			
			default:
				cerr << "Error: "  << eq->equationType() << endl;
				throw "Not implemented yet!!";
			
		}		
	}
}


MMO_ToMicroModelica newMMO_ToMicroModelica(MMO_Class  c )
{
	return new MMO_ToMicroModelica_(c);
}
