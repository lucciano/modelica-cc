
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
#include <util/solve.h>
#include <parser/parse.h>
#include <ast/equation.h>
#include <string.h>
#include <stdio.h>
#include <sstream>
#include <cassert>
#include <fstream> 
#include <util/ginac_interface.h>
#include <causalize/debug.h>
#include <causalize/causalization_strategy.h>
#include <causalize/compref_occurrence_traverse.h>

MMO_EquationList EquationSolver::solve(MMO_EquationList eqs, AST_ExpressionList crs, AST_ExpressionList all_unknowns) {
  static int fsolve=1;
  ConvertToGiNaC tog(NULL); // No var symbol table needed for now
  ConvertToExpression toe;

  GiNaC::lst eqns, vars;
  AST_ExpressionListIterator it_cr,it2_cr;
  AST_EquationListIterator it_eq;
  foreach(it_cr,crs) {
    if (current_element(it_cr)->expressionType()==EXPCOMPREF) {
      AST_Expression_ComponentReference cr=current_element(it_cr)->getAsComponentReference();
      DEBUG('c', "Solving for variable %s ",cr->print().c_str());
      vars.append(tog.getSymbol(cr));
    } else if (current_element(it_cr)->expressionType()==EXPDERIVATIVE) {
      AST_Expression_Derivative der=current_element(it_cr)->getAsDerivative();
      vars.append(tog.getSymbol(der));
      DEBUG('c', "Solving for derivative %s ",der->print().c_str());
    }
  }
  foreach(it_eq,eqs) {
    AST_Equation_Equality eq = current_element(it_eq)->getAsEquality();
  
    DEBUG('c', "Using equaiton %s",eq->print().c_str());
    GiNaC::ex left=tog.convert(eq->left());
    GiNaC::ex right=tog.convert(eq->right());
    eqns.append(left==right);
  }

  try {
    /* System is linear */
    GiNaC::ex solved= lsolve(eqns, vars);
    AST_EquationList ret=newAST_EquationList();
    if (solved.nops()==0) {
      cerr << "EquationSolver: cannot solve equation" << eqns<< endl;
      abort();
    }
    for(int i=0; i<solved.nops();i++) 
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
        if (occur(current_element(it_cr),eq)) {
          // Check which unknown they use
          CompRefOccurrenceTraverse *occurrenceTraverse = new CompRefOccurrenceTraverse(current_element(it_cr));
          bool already_arg=false;
          foreach(it2_cr,args) {
            // If it is already an argument do not add it twice
            if (occurrenceTraverse->foldTraverse(current_element(it2_cr))) {
              already_arg=true;
              break;
            }
          }     
          if (already_arg)
            continue;
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
        }
      }
    }
    AST_Expression call= newAST_Expression_Call(_S(s.str()),NULL,args);
    AST_Equation e= newAST_Equation_Equality(newAST_Expression_OutputExpressions(crs),call);
    int index=0;
    const int size=AST_Length(eqs);
    const int args_size=AST_Length(args);
    std::fstream fs ("test.c", std::fstream::out);
    fs << "#include <gsl/gsl_multiroots.h>"<< endl;
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
    fs << "  gsl_vector *__x = gsl_vector_alloc (" << size << ");" << endl;
    fs << "  __F.n = " << size <<";"<<endl;
    fs << "  __F.f = fsolve" << fsolve <<"_eval;" << endl;
    fs << "  double __args[" << args_size << "];"<< endl; 
    index=0;
    foreach(it_cr,args) {
      fs << "  __args[" << index++ << "]=" << current_element(it_cr) << ";" << endl;
    }
    fs << "  __F.params  = __args;" << endl;
    index=0;
    foreach(it_cr,crs) {
      fs << "  //gsl_vector_set (__x, 0, "<< current_element(it_cr) << ");" << endl;
    }
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
    fs << "  gsl_multiroot_fsolver_free (__s);"<< endl;
    fs << "  gsl_vector_free (__x);"<< endl;
    fs << "}" << endl;
    fsolve++;
    return newAST_SimpleList(e);
  }
}

