CXX = g++
override CXXFLAGS += -std=c++17 -O3 -Wall -I ~/numerical/numpy -I .
HEADER = $(wildcard ./aye/*.hpp)
SRC = $(wildcard ./test/test_*.cpp)
EXC = $(basename $(SRC))

FMT = .bmp
TXT = $(wildcard ./images/*.txt)
IMG = $(addsuffix $(FMT), $(basename $(TXT)))

.PHONY: clean all render open

all: $(EXC)

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

