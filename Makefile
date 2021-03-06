CXXFLAGS = -DYY_MCC_Parser_DEBUG -I. -g -Wno-write-strings -Wall

OBJS_COMMON := parser/mocc_parser.o \
        parser/mocc_scanner.o \
        ast/ast_builder.o \
        ast/ast_node.o \
        ast/class.o  \
        ast/composition.o \
        ast/element.o \
        ast/equation.o \
        ast/expression.o \
        ast/modification.o \
        ast/statement.o \
        ast/stored_definition.o  \
        parser/parse.o 

all:bin/mmo bin/causalize bin/antialias test/causalize/cycles_identification_strategy_test test/causalize/for_unrolling/process_for_equations_test test/util/EvalExpTest

include mmo/Makefile.include
include causalize/Makefile.include
include antialias/Makefile.include
include test/causalize/Makefile.include
include test/causalize/for_unrolling/Makefile.include
include test/util/Makefile.include

%.o: %.cpp %.h
	$(CXX) -c $(CXXFLAGS) $*.cpp -o $*.o


docs:
	doxygen 

parser/mocc_parser.cpp: parser/mocc.y
	bison++ -t -d -v -hparser/mocc_parser.h -o parser/mocc_parser.cpp parser/mocc.y

parser/mocc_scanner.cpp: parser/mocc.lex parser/mocc_parser.cpp
	flex++ -d -oparser/mocc_scanner.cpp parser/mocc.lex

clean:
	rm -rf $(OBJS_ANTIALIAS) $(OBJS_COMMON) $(OBJS_MMO) $(OBJS_CAUSALIZE) $(OBJS_TEST_CAUSALIZE) $(OBJS_TEST_UTIL) parser/mocc_parser.cpp parser/mocc_scanner.cpp parser/mocc_parser.h bin/mmo bin/causalize bin/antialias test/causalize/cycles_identification_strategy_test test/util/EvalExpTest 



	
