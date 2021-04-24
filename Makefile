CXX = g++
override CXXFLAGS += -std=c++17 -O3 -Wall -I ~/numerical/numpy -I .
HEADER = $(wildcard ./eyeball/*.hpp)
SRC = $(wildcard ./test/*.cpp)
EXC = $(basename $(SRC))

.PHONY: clean all run txt render open

all: run

%: %.cpp $(HEADER)
	$(CXX) $(CXXFLAGS) -o $@ $<

run: $(EXC)
	for name in $(EXC); do $$name; done

render: images/*.txt
	@ls $^ | xargs -L 1 ./io to_image

open:
	open ./images/*.jpg

clean:
	rm $(EXC)

