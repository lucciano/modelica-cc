#include <iostream>

#include <parser/parse.h>

using namespace std;

int main(int argc, char ** argv)
{
  int r;
  if (argc<2) {
    cerr << "Usage:\n\tmcc file.mo\n";
    return -1;
  }
  AST_StoredDefinition sd = parseFile(argv[1],&r);
  if (r==0) // Parsed ok
    cerr << sd;
  return 0;
}
