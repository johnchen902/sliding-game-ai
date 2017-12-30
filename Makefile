CXXFLAGS += -O3
LDFLAGS += -lncurses

.PHONY: all clean

all: main test

main: main.cpp combo.h data.h idastar.h
	$(CXX) $(CXXFLAGS) $(LDFLAGS) main.cpp -o main

test: test.cpp combo.h data.h idastar.h
	$(CXX) $(CXXFLAGS) $(LDFLAGS) test.cpp -o test

clean:
	$(RM) main test
