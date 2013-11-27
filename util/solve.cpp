
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

#include <string.h>
#include <stdio.h>
#include <sstream>
#include <cassert>
#include <fstream> 

#include <ast/ast_builder.h>
#include <parser/parse.h>
#include <ast/equation.h>
#include <causalize/causalization_strategy.h>
#include <causalize/compref_occurrence.h>
#include <util/ginac_interface.h>
#include <util/solve.h>
#include <util/debug.h>

AST_Class makeFsolve(AST_String name, int args_size, int size) {
  AST_DeclarationList in=NULL,out=NULL;
  AST_ElementList el = newAST_ElementList();
  if (args_size) {
    for (int i=0;i<args_size;i++) {
      stringstream s(ios_base::out);
      s << "i" << i;
      AST_Declaration d=newAST_Declaration(newAST_String(s.str()),newAST_ExpressionList(),newAST_ModificationNull());
      if (i==0) 
        in = newAST_DeclarationList(d);
      else
        AST_ListAppend(in,d);
    }
    AST_Element_Component ec_in=newAST_Element_Component(in,newAST_String("Real"),newAST_TypePrefix(TP_INPUT),newAST_ExpressionList());
    AST_ListAppend(el,AST_Element_ComponentToElement(ec_in));
  }
  for (int i=0;i<size;i++) {
      stringstream s(ios_base::out);
      s << "o" << i;
      AST_Declaration d=newAST_Declaration(newAST_String(s.str()),newAST_ExpressionList(),newAST_ModificationNull());
      if (i==0) 
        out = newAST_DeclarationList(d);
      else
        AST_ListAppend(out,d);
  }
  AST_Element_Component ec_out=newAST_Element_Component(out,newAST_String("Real"),newAST_TypePrefix(TP_OUTPUT),newAST_ExpressionList());
  AST_ListAppend(el,AST_Element_ComponentToElement(ec_out));
  AST_Composition c=newAST_Composition(el);
  AST_ArgumentList al=newAST_ArgumentList();
  AST_ListAppend(al,newAST_ElementModification(newAST_String("Library"),newAST_Modification_Equal(newAST_Expression_String(newAST_String("gsl")))));
  AST_ListAppend(al,newAST_ElementModification(newAST_String("Include"),newAST_Modification_Equal(newAST_Expression_String(newAST_String("#include \\\"buck_external_functions.c\\\"")))));
  c=AST_Composition_SetExternalAnnotation(c,newAST_ExternalCall(newAST_String("C"),al),NULL);
  AST_Class r=AST_Class_SetPrefixEncapsulated(newAST_Class(name,c),AST_ClassPrefix_Function(0),false);
  return r; 
}

MMO_EquationList EquationSolver::solve(AST_String name,
                                       MMO_EquationList eqs,
                                       AST_ExpressionList crs,
                                       AST_ExpressionList all_unknowns,
                                       AST_ClassList cl,
                                       VarSymbolTable symbolTable) {
  static int fsolve=1;
  ConvertToGiNaC tog(NULL); // No var symbol table needed for now
  ConvertToExpression toe;

  const int size=AST_Length(eqs);
  GiNaC::lst eqns, vars;
  AST_ExpressionListIterator it_cr,it2_cr;
  AST_EquationListIterator it_eq;
  foreach(it_cr,crs) {
    if (current_element(it_cr)->expressionType()==EXPCOMPREF) {
      AST_Expression_ComponentReference cr=current_element(it_cr)->getAsComponentReference();
      DEBUG('s', "Solving for variable %s\n",cr->print().c_str());
      vars.append(tog.getSymbol(cr));
    } else if (current_element(it_cr)->expressionType()==EXPDERIVATIVE) {
      AST_Expression_Derivative der=current_element(it_cr)->getAsDerivative();
      vars.append(tog.getSymbol(der));
      DEBUG('s', "Solving for derivative %s\n",der->print().c_str());
    }
  }
  foreach(it_eq,eqs) {
    AST_Equation_Equality eq = current_element(it_eq)->getAsEquality();
  
    DEBUG('s', "Using equaiton %s",eq->print().c_str());
    GiNaC::ex left=tog.convert(eq->left());
    GiNaC::ex right=tog.convert(eq->right());
    eqns.append(left==right);
  }

  try {
    /* System is linear */
    if (size>5) throw std::logic_error("");
    GiNaC::ex solved= lsolve(eqns, vars,GiNaC::solve_algo::gauss);
    AST_EquationList ret=newAST_EquationList();
    if (solved.nops()==0) {
      cerr << "EquationSolver: cannot solve equation" << eqns<< endl;
      abort();
    }
    for(unsigned int i=0; i<solved.nops();i++) 
      AST_ListAppend(ret,newAST_Equation_Equality(toe.convert(solved.op(i).op(0)),toe.convert(solved.op(i).op(1))));
    return ret;
  } catch (std::logic_error) {
    /* System is non linear */
    stringstream s(ios_base::out);
    s << "fsolve" << fsolve;
    /* Compute input arguments */
    AST_ExpressionList args=newAST_ExpressionList();
    foreach(it_eq,eqs) {
      // For each equation
      AST_Equation_Equality eq = current_element(it_eq)->getAsEquality();
      foreach(it_cr,all_unknowns) {
        if (occur(current_element(it_cr),eq, symbolTable)) {
          // Check which unknown they use
          CompRefOccurrenceTraverse *occurrenceTraverse = new CompRefOccurrenceTraverse(current_element(it_cr), symbolTable);
          bool already_arg=false;
          foreach(it2_cr,args) {
            // If it is already an argument do not add it twice
            if (occurrenceTraverse->foldTraverse(current_element(it2_cr))) {
              already_arg=true;
              break;
            }
          }     
          if (already_arg) {
            delete occurrenceTraverse;
            continue;
          }
          bool found=false;
          foreach(it2_cr,crs) { 
            // If is not one of the output variables and it is used, it is an input argument for the fsolve function
            if (occurrenceTraverse->foldTraverse(current_element(it2_cr))) {
              found=true;
              break;
            }
          }
          if (!found)
              AST_ListAppend(args,current_element(it_cr));
          delete occurrenceTraverse;
       } else if (current_element(it_cr)->expressionType()==EXPDERIVATIVE) {
          /* If unknown is a derivative check if the arg is an argument for the loop */
          AST_Expression der_arg=AST_ListFirst(current_element(it_cr)->getAsDerivative()->arguments());
          if (occur(der_arg,eq, symbolTable)) {
            CompRefOccurrenceTraverse *occurrenceTraverse = new CompRefOccurrenceTraverse(der_arg, symbolTable);
            bool already_arg=false;
            foreach(it2_cr,args) {
              // If it is already an argument do not add it twice
              if (occurrenceTraverse->foldTraverse(current_element(it2_cr))) {
                already_arg=true;
                break;
              }
            }     
            if (already_arg) {
              delete occurrenceTraverse;
              continue;
            }
            bool found=false;
            foreach(it2_cr,crs) { 
              // If is not one of the output variables and it is used, it is an input argument for the fsolve function
              if (occurrenceTraverse->foldTraverse(current_element(it2_cr))) {
                found=true;
                break;
              }
            }
            if (!found)
              AST_ListAppend(args,der_arg);
            delete occurrenceTraverse;
          }
        }
      }
    }
    AST_Expression call= newAST_Expression_Call(_S(s.str()),NULL,args);
    AST_Equation e= newAST_Equation_Equality(newAST_Expression_OutputExpressions(crs),call);
    const int args_size=AST_Length(args);
    int index=0;
    string file_name = toStr(name) + "_external_functions.c";
    std::fstream fs (file_name.c_str(),(fsolve==1 ? std::fstream::out : std::fstream::out | std::fstream::app));
    if (fsolve==1) {
      fs << "#include <gsl/gsl_multiroots.h>"<< endl;
      fs << "#include \"" << toStr(name) << "_parameters.h\""<< endl;
    }
    fs << "int fsolve" << fsolve << "_eval(const gsl_vector * __x, void * __p, gsl_vector * __f) {" << endl;
    fs << "  double *args=(double*)__p;" << endl;
    foreach(it_cr,crs) {
      fs  << "  const double " << current_element(it_cr) << " = gsl_vector_get(__x," << index++ << ");" << endl;
    }
    index =0;
    foreach(it_cr,args) {
      fs << "  const double " << current_element(it_cr) << " = args[" << index++ << "];" << endl;
    }
    index = 0;
    foreach(it_eq,eqs) {
      AST_Equation_Equality eq = current_element(it_eq)->getAsEquality();
      AST_Expression e = newAST_Expression_BinOp(eq->right(),eq->left(),BINOPSUB);
      fs  << "  gsl_vector_set (__f, " << index++ << "," << e << ");" << endl;
    }
    fs << "   return GSL_SUCCESS;"<< endl;
    fs << "}"<< endl;
    fs << "void fsolve" << fsolve << "(";

    index =0;
    foreach(it_cr,args) {
      fs << "double " << current_element(it_cr) << (index++<args_size-1 ? "," : "");
    }
    if (args_size)
      fs << ",";
    index=0;
    foreach(it_cr,crs) {
      fs << "double *" << current_element(it_cr) << (index++<size-1 ? "," : "");
    }
    fs << ") {" << endl;
    fs << "  size_t __iter = 0;" << endl;
    fs << "  int __status;" << endl;
    fs << "  const gsl_multiroot_fsolver_type *__T = gsl_multiroot_fsolver_hybrid;" << endl;
    fs << "  gsl_multiroot_fsolver *__s = gsl_multiroot_fsolver_alloc (__T, " << size << ");" << endl;
    fs << "  gsl_multiroot_function __F;" << endl;
    fs << "  static gsl_vector *__x = NULL; " << endl;
    fs << "  if (__x==NULL) { " << endl;
    fs << "    __x=gsl_vector_alloc (" << size << ");" << endl;
    index=0;
    foreach(it_cr,crs) {
      fs << "    gsl_vector_set (__x, " << index++ << ",0);" << endl;
    }
    fs << "  }" << endl;
    fs << "  __F.n = " << size <<";"<<endl;
    fs << "  __F.f = fsolve" << fsolve <<"_eval;" << endl;
    fs << "  double __args[" << args_size << "];"<< endl; 
    index=0;
    foreach(it_cr,args) {
      fs << "  __args[" << index++ << "]=" << current_element(it_cr) << ";" << endl;
    }
    fs << "  __F.params  = __args;" << endl;
    index=0;
    fs << "  gsl_multiroot_fsolver_set (__s, &__F,__x);" << endl;
    fs << "  do"<< endl;
    fs << "  {" << endl;
    fs << "    __iter++;" << endl;
    fs << "    __status = gsl_multiroot_fsolver_iterate (__s);" << endl;
    fs << "    if (__status)   /* check if solver is stuck */" << endl;
    fs << "      break;" << endl;
    fs << "    __status = gsl_multiroot_test_residual (__s->f, 1e-7);"<< endl;
    fs << "  } while (__status == GSL_CONTINUE && __iter < 1000);" << endl;
    index=0;
    foreach(it_cr,crs) {
      fs << "  " << current_element(it_cr) << "[0] = gsl_vector_get(__s->x," << index++ << ");" << endl;
    }
    index=0;
    foreach(it_cr,crs) {
      fs << "  gsl_vector_set (__x, " << index << ", gsl_vector_get(__s->x," << index << "));" << endl;
      index++;
    }
    fs << "  gsl_multiroot_fsolver_free (__s);"<< endl;
    fs << "}" << endl;
    AST_ListAppend(cl,makeFsolve(newAST_String(s.str()),args_size,size));
    return newAST_SimpleList(e);
  }
}

