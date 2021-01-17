CXX := g++
TARGET := ../bin/hw5
CXXFLAGS := -std=c++11 -O3 -Wall -Wextra -fopenmp
SRC_DIRS := .
SRCS := $(wildcard $(SRC_DIRS:=/*.cpp))
OBJS := $(SRCS:.cpp=.o)
DEPS = $(OBJS:.o=.d)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ -lpthread -fopenmp

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

clean:
	rm -rf $(TARGET) $(OBJS) $(DEPS)

ifeq (test,$(firstword $(MAKECMDGOALS)))
  TEST_CASE_ID := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  $(eval $(TEST_CASE_ID):;@:)
endif

test: $(TARGET)
	@echo test on case $(TEST_CASE_ID)
	/usr/bin/time -p ./$(TARGET) ../testcase/$(TEST_CASE_ID).modified.txt ../output/$(TEST_CASE_ID).result
	../verifier/verify ../testcase/$(TEST_CASE_ID).modified.txt ../output/$(TEST_CASE_ID).result

.PHONY: all clean test
-include $(DEPS)