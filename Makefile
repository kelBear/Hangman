CXXFLAGS= -O -Wall -std=c++0x -MMD
CXX=g++
OBJECTS = hangman.o
DEPENDS = ${OBJECTS:.o=.d}
EXEC=hangman

$(EXEC): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(OBJECTS) 


clean:
	rm -f *.o $(EXEC) $(TESTS) $(DEPENDS)

-include ${DEPENDS}