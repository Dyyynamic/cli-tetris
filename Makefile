TARGET = tetris-cli

CXX = g++

CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic

LIBS = -lncurses

main:
	$(CXX) $(CXXFLAGS) main.cpp -o $(TARGET) $(LIBS)

.PHONY: clean
clean:
	rm -f $(TARGET)
