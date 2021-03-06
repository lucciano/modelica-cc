
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

#include <fstream>
#include <cstdlib>
#include <parser/parse.h>
#include <ast/stored_definition.h>
#include <ast/class.h>
#include <parser/mocc_parser.h>

using namespace std;

AST_StoredDefinition parseFile(string filename, int *r) {
  fstream in;   
  int ret;
  MCC_Parser parser(false);
  if (filename.size()) {
    in.open(filename.c_str(),fstream::in);
    if (in.fail()) {
      cerr << "Could not open file " << filename.c_str() << endl;
      exit(-1);
    }
    ret = parser.parseFile(&in);
  } else { // read from stdin
    ret = parser.parseFile(&cin);
  }
  if (ret==0) {
    *r=0;
    in.close();
    return parser.root();
  }
  *r=-1;
  in.close();
  return NULL;
}

AST_Expression parseExpression(string exp, int *r) {
  fstream in;   
  in.open("/tmp/t",fstream::out);
  in << "model A equation x = " << exp << "; end A;";
  in.close();
  int ret;
  AST_Class c=parseClass("/tmp/t",&ret);
  if (ret==0) {
    *r=0;
    return c->composition()->compositionList()->front()->getEquationsAlgs()->getEquations()->front()->getAsEquality()->right();
  }
  *r=-1;
  return NULL;
}


AST_Class parseClass(string filename, int *r) {
  AST_StoredDefinition sd = parseFile(filename,r);
  if (r[0]==0)
    return sd->models()->front();
  return NULL;

} 
