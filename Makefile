TARGET = tetris-cli

CXX = g++

CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic

SRCS = main.cpp \
	   GameController.cpp \
	   GameState.cpp \
	   MainMenuState.cpp \
	   GamePlayState.cpp \
	   GameOverState.cpp

LIBS = -lncurses

main:
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(LIBS)

.PHONY: clean
clean:
	rm -f $(TARGET)
