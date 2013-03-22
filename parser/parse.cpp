#include <fstream>
using namespace std;
#include <parser/parse.h>
#include <parser/mcc_parser.h>
AST_StoredDefinition parseFile(string filename, int *r) {
  fstream in;   
  parser = new MCC_Parser(false);
  in.open(filename.c_str(),fstream::in);
  int ret = parser->parseFile(&in);
  if (ret==0) {
    *r=0;
    delete parser;
    in.close();
    return root;
  }
  *r=-1;
  delete parser;
  in.close();
  return NULL;
}

