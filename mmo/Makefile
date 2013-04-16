CXXFLAGS = -DYY_MCC_Parser_DEBUG -I. -g

OBJS_MMO := $(OBJS_COMMON) \
            ../mmo/main.o \
            ../mmo/traverse_example.o \
            ../mmo/mmo_class.o \
            ../util/ast_util.o

mmo: $(OBJS_MMO)
	g++ $(CXXFLAGS) -o mmo/mmo $(OBJS_MMO)



	
