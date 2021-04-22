CXX = g++
override CXXFLAGS += -std=c++2a -O3 -Wall -I ~/numerical/numpy
HEADER = $(wildcard ./numpy/*.hpp)
SRC = $(wildcard ./test/*.cpp)
EXC = $(basename $(SRC))
CPPTEST = EXC
# NOTPYTEST = test_iter
# PYTEST = $(filter-out $(addprefix %/, $(NOTPYTEST)), $(EXC))
# CPPTEST = $(filter-out $(PYTEST), $(EXC))
# override pypp += 

.PHONY: clean all run #py++ ufunc

all: test

%: %.cpp $(HEADER)
	$(CXX) $(CXXFLAGS) -o $@ $<

test: run #py++

run: $(CPPTEST)
	for name in $(CPPTEST); do $$name; done

# py++:
# 	for name in $(notdir $(PYTEST)); do ./py++ $$name $(pypp) ; done

# UFUNC := numpy/ufunc_instances.hpp
# ufunc: $(UFUNC)
# $(UFUNC): ufunc_generate.py ufunc.txt
# 	python $^ $@

clean:
	rm $(EXC)

