CXX = g++
override CXXFLAGS += -std=c++17 -O3 -Wall -I ~/numerical/numpy -I .
HEADER = $(wildcard ./eyeball/*.hpp)
SRC = $(wildcard ./test/*.cpp)
EXC = $(basename $(SRC))

FMT = .bmp
TXT = $(wildcard ./images/*.txt)
IMG = $(addsuffix $(FMT), $(basename $(TXT)))

.PHONY: clean all run txt render open

all: run

%: %.cpp $(HEADER)
	$(CXX) $(CXXFLAGS) -o $@ $<

render: $(IMG)
# @ls $^ | xargs -L 1 ./io to_image

%$(FMT): %.txt
	./io to_image $< -o $@

open:
	open ./images/*$(FMT)

clean:
	rm $(EXC)

