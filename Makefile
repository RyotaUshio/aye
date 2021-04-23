CXX = g++
override CXXFLAGS += -std=c++17 -O3 -Wall -I ~/numerical/numpy -I .
HEADER = $(wildcard ./vision/*.hpp)
SRC = $(wildcard ./test/*.cpp)
EXC = $(basename $(SRC))

.PHONY: clean all run

all: run

%: %.cpp $(HEADER)
	$(CXX) $(CXXFLAGS) -o $@ $<

run: $(EXC)
	for name in $(EXC); do $$name; done

clean:
	rm $(EXC)

