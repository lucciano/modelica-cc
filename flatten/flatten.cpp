
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
#include <ast/expression.h>
#include <flatten/flatten.h>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <set>


Flatten::Flatten()
{
  AST_String name = newAST_String("Real");
  AST_Class real = newAST_Class(name);
  real->setBasic();
  types.addSymbol("Real", real);
}

int Flatten::flatten(AST_StoredDefinition sd)
{
  AST_ClassList cl = sd->models();
  AST_ClassListIterator it;
  for (it=cl->begin();it!=cl->end();it++) {
    types.addSymbol(current(it)->name(),current(it));
  }
 /* Flat in the tsort order */
  AST_StringList ls=tsort(cl->back()); 
  /* Dump types table */
  types.dump(); 
  AST_StringListIterator stit;
  for (stit=ls->begin();stit!=ls->end();stit++) {
    AST_Class ty = types.lookup(current(*stit));
    doFlat(ty);
  }
  
  /* Print all model */
  
  for (it=cl->begin();it!=cl->end();it++) {
    cerr << "************************   Class  " << current(it)->name() << " *****************************" << endl;
    AST_Class ty = types.lookup(current(it)->name());
    AST_EquationListIterator eqit;
    AST_EquationList eqs = ty->getEquations();
    AST_ComponentList comps = ty->getComponents();
    AST_ComponentListIterator compit;
    foreach(compit,comps)  {
      if (current(compit)->isParameter())
        continue;
      AST_Class ty = types.lookup(current(compit)->type());
  //    if (ty->isBasic())
        cerr << *current(compit);
    } 
    cerr << "*************************************************************************" << endl;
    foreach(eqit,eqs) 
      //if (current(eqit)->equationType()!=EQCONNECT) // Skip connect equations
        cerr << *current(eqit);
    cerr << "*************************************************************************" << endl;
  }
  return 0;
  
}

void Flatten::preFlat(AST_Class c) { 
  AST_Composition comp = c->composition();
  AST_CompositionElementList cel = comp->compositionList();
  AST_CompositionElementListIterator it;
  foreach(it,cel) {
    AST_EquationListIterator eqit;
    AST_CompositionElement e = current(it);
    AST_EquationList eqs = e->getEquations();
    foreach(eqit,eqs) 
      c->addEquation(current(eqit));
  }
  AST_ElementList el = comp->elementList();
  AST_ElementListIterator elit;
  foreach(elit,el) {
    switch (current(elit)->elementType()) {
      case COMPONENT:
        c->addComponent(current(elit)->getAsComponent());
        break;
      case EXTENDS:
        c->addExtends(current(elit)->getAsExtendsClause()->name());
        break;
      case ELCLASS:
        c->addClass(current(elit)->getAsClass()->getClass());
        break;
    }
  }
}

void Flatten::doFlat(AST_Class c) { 
  /* First check all components */
  AST_ComponentList cl = c->getComponents();
  AST_ComponentListIterator it;
  AST_StringListIterator stit;
  AST_StringList stlist = c->getExtends();
  AST_ClassList sub_cl = c->getClasses();
  AST_ClassListIterator clit;
  foreach(clit,sub_cl) {
    cerr << "Flattening " << current(clit)->name() << endl;
    preFlat(current(clit));
    doFlat(current(clit));
    types.addSymbol(current(clit)->name(),current(clit));
    
  }
  foreach(stit,stlist) { // First expands extends clauses 
    AST_Class ty = types.lookup(*current(stit));
    AST_EquationListIterator eqit;
    AST_EquationList eqs = ty->getEquations();
    // Include type's equation in the flaten class 
    foreach(eqit,eqs) {
      AST_Equation neweq = duplicateEquation(current(eqit));
      c->addEquation(neweq);
    }
    AST_ComponentList comps = ty->getComponents();
    AST_ComponentListIterator compit;
    foreach(compit,comps) {
      /* Must duplicate Component!!!! */
      c->addComponent(current(compit));
      current(compit)->setInherited();
      current(compit)->setOrigin(ty);
    }
  }
  foreach(it,cl) { // Then add composition elements
      //cerr << current(it)->name() << ":" << current(it)->type() << endl;
      if (current(it)->isInherited())
        continue;
      AST_Class ty = types.lookup(current(it)->type());
      AST_EquationListIterator eqit;
      AST_EquationList eqs = ty->getEquations();
      /* Include type's equation in the flaten class */
      foreach(eqit,eqs) {
        AST_Equation neweq = duplicateEquation(current(eqit));
        replaceComponentRefs(current(it)->name(),neweq,ty);
        c->addEquation(neweq);
      }
      /* Include type's component in the flat class */
      AST_ComponentList comps = ty->getComponents();
      AST_ComponentListIterator compit;
      foreach(compit,comps) {
        AST_Component compc = (replaceComponent(current(it)->name(),current(compit)));
        compc->setOrigin(ty);
        c->addComponent(compc);
      }
  }
  // Expand connect equations
  AST_EquationListIterator eqit;
  AST_EquationList eqs = c->getEquations();
  foreach(eqit,eqs) { 
    if (current(eqit)->equationType()==EQCONNECT) { 
      AST_EquationConnect ce = current(eqit)->getAsConnect();
      expandConnections(ce,c);
    }
  }
  
}

void Flatten::replaceComponentRefs(string name, AST_Expression e, AST_Class ty) {
  switch (e->expressionType()) {
    case EXPCOMPREF:
    {
      AST_Expression_ComponentReference cr = e->getAsComponentRef();
      string n = cr->name();
      if (isLocalTo(n,ty)) {
        name.append(".");
        name.append(n);
        cr->setName(name);
      }
      break;
    }
    case EXPDERIVATIVE:
    {
      AST_Expression_Derivative der = e->getAsDerivative();
      AST_ExpressionList el         = der->arguments();
      AST_ExpressionListIterator it;
      foreach(it,el) {
        replaceComponentRefs(name,current(it),ty);
      }
      
      break;
    }
    /*
    case BINOP:
    {
      AST_Expression_Addition bo = e->getAsBinOpAdd();
      replaceComponentRefs(name,bo->e1(), ty);
      replaceComponentRefs(name,bo->e2(), ty);
      break;
    }
     */ 
  }
}
void Flatten::replaceComponentRefs(string name, AST_Equation e, AST_Class c) {
  switch (e->equationType()) {
    case EQEQUALITY:
      // This should duplicate the equation
      replaceComponentRefs(name,e->getAsEquality()->left(),c);
      replaceComponentRefs(name,e->getAsEquality()->right(),c);
      
    break;
  }
}

bool Flatten::isLocalTo(string id,AST_Class ty) {
  AST_ComponentList cl = ty->getComponents();
  AST_ComponentListIterator it;
  foreach(it,cl) {
      if (current(it)->name()==id) return true;
  }
  return false;
}



AST_Component Flatten::replaceComponent(string name, AST_Component comp) {
  AST_String n = newAST_String(comp->name());
  n = AST_StringDotAppend(newAST_String(name),n);
  AST_DeclarationList dl = newAST_DeclarationList(newAST_Declaration(n));
  AST_Component c  = newAST_Component(dl,newAST_String(comp->type()),comp->typePrefix());
  c->setInherited();
  return c;
}

void Flatten::expandConnections(AST_EquationConnect e, AST_Class c) {
  AST_ComponentList comps = c->getComponents();
  AST_ComponentListIterator compit;
}

AST_StringList Flatten::tsort(AST_Class c) {
  AST_StringList ls=newAST_StringList();
  system("rm /tmp/tsort");
  /* TSORT */
  set<string> mustFlat;
  set<string> alreadyFlat;
  mustFlat.insert(c->name());
  fstream ts;
  ts.open("/tmp/tsort",fstream::out| fstream::app);
  while (!mustFlat.empty()) {
    /* Take first element */
    string fl_name = current(mustFlat.begin());
    mustFlat.erase(mustFlat.begin());
    /* If it has already been pre-flattend, skip it */
    if (alreadyFlat.find(fl_name)!=alreadyFlat.end())
      continue;
    alreadyFlat.insert(fl_name);
    /* Do Pre-flat and tsort */
   cerr << "Look for " << fl_name << endl;
    AST_Class fl = types.lookup(fl_name);
    cerr << "Result " << fl;
    if (fl->isBasic()) 
      continue;
    preFlat(fl);
    AST_ComponentList cl = fl->getComponents();
    AST_ComponentListIterator compit;
    foreach(compit,cl)  {
      string flat;
      /* If is a compound type */
      if (current(compit)->type().find_first_of(".")!=string::npos) {
        flat = current(compit)->type().substr(0,current(compit)->type().find_first_of("."));
      } else {
        flat = current(compit)->type();
      }
      ts << fl->name() << " " << flat << endl;
      mustFlat.insert(flat);
    }
    AST_StringListIterator stit;
    AST_StringList stlist = fl->getExtends();
    foreach(stit,stlist) { 
      ts << fl->name() << " " << current(*stit) << endl;
      mustFlat.insert(current(*stit));
    }
  }
  ts.close();
  system("tsort /tmp/tsort > /tmp/tsort_out");

  
  
  /* Read tsort result */
  fstream ts_res;
  ts_res.open("/tmp/tsort_out",fstream::in);
  list<string*>::iterator stit;
  while (!ts_res.eof()) {
    string *s = new string();;
    ts_res >> *s;
    if(ts_res.eof())
      break;
    ls->push_front(s);
  }
  ts_res.close();
  return ls; 
}

