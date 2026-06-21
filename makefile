CXX = clang++

CXXFLAGS = -std=c++23 -Iinclude -I/opt/homebrew/include
LDLIBS   = -L/opt/homebrew/lib -lgtest -lgtest_main -lpthread

SRC_MAIN = src/main.cpp

TEST_SRCS = $(wildcard tests/*.cpp) 

OBJ_MAIN = $(SRC_MAIN:.cpp=.o)

main: $(OBJ_MAIN)
	$(CXX) $^ -o $@ $(CXXFLAGS)
	./$@

test: $(TEST_SRCS)
	$(CXX) $^ -o test $(CXXFLAGS) $(LDLIBS)
	./test

ui:
	@echo "Сборка Qt интерфейса через CMake..."
	mkdir -p ui_ui/build
	cd ui_ui/build && cmake .. && $(MAKE)
	@echo "Запуск интерфейса..."
	open ui_ui/build/ui_ui.app

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_MAIN) main test
	rm -rf ui_ui/build
	find . -name ".DS_Store" -type f -delete

.PHONY: main test clean ui