CXX = g++
override CXXFLAGS += -std=c++17 -O3 -Wall -I ~/numerical/numpy -I .
HEADER = $(wildcard ./eyeball/*.hpp)
SRC = $(wildcard ./test/*.cpp)
EXC = $(basename $(SRC))

TXT = $(wildcard ./images/*.txt)
IMG = $(addsuffix .jpg, $(basename $(TXT)))

.PHONY: clean all run txt render open

all: run

%: %.cpp $(HEADER)
	$(CXX) $(CXXFLAGS) -o $@ $<

render: $(IMG)
# @ls $^ | xargs -L 1 ./io to_image

%.jpg: %.txt
	./io to_image $< -o $@

open:
	open ./images/*.jpg

clean:
	rm $(EXC)

