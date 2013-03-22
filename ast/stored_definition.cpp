#include <ast/ast_builder.h>
#include <ast/stored_definition.h>

AST_StoredDefinition_:: AST_StoredDefinition_(AST_ClassList ml, AST_String within): _model_list(ml), _within(within) {
};
ostream & operator<<(ostream &os , const AST_StoredDefinition &sd )  {
  os << *sd;
  return os;

}
ostream & operator<<(ostream &os , const AST_StoredDefinition_ &sd )  {
  AST_ClassListIterator it;
  AST_ClassList         cl = sd.models();
    if (sd.within()!=NULL)
      os << "within " << sd.within()<< endl; 
    foreach(it,cl)
      os << current(it);
    return os;
}

