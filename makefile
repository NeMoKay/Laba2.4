CXX = clang++

CXXFLAGS = -std=c++23 -Iinclude -I/opt/homebrew/include
LDLIBS   = -L/opt/homebrew/lib -lgtest -lgtest_main -lpthread

SRC_MAIN = src/main.cpp
SRC_SERVER = src/web_server.cpp

TEST_SRCS = $(wildcard tests/*.cpp) 

OBJ_MAIN = $(SRC_MAIN:.cpp=.o)

main: $(OBJ_MAIN)
	$(CXX) $^ -o $@ $(CXXFLAGS)
	./$@

server: 
	$(CXX) $(SRC_SERVER) -o web_interface $(CXXFLAGS) -lpthread
	@echo "Opening browser in 1 second..."
	@(sleep 1 && open http://localhost:8080) &
	./web_interface

test: $(TEST_SRCS)
	$(CXX) $^ -o test $(CXXFLAGS) $(LDLIBS)
	./test

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_MAIN) main test
	rm -f web_interface

.PHONY: main test clean run_server clean_web server