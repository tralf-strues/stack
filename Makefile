Options = -Wall -Wpedantic -DSTACK_DEBUG_MODE

SrcDir = src
BinDir = bin
LibDir = libs

$(BinDir)\stack.a : $(BinDir)\stack.o $(LibDir)\log_generator.a $(LibDir)\log_generator.h
	ar ru $(BinDir)\stack.a $(BinDir)\stack.o $(LibDir)\log_generator.a
	
$(BinDir)\stack.o : $(SrcDir)\stack.cpp $(SrcDir)\stack.h
	g++ -o $(BinDir)\stack.o -c $(SrcDir)\stack.cpp $(Options)