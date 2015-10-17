OBJS = src/pung.cpp
OBJ_NAME = build/pung

CC = clang++
C_FLAGS = -Wall -Wextra -std=c++11
L_FLAGS = -lsfml-graphics -lsfml-window -lsfml-system -lBox2D
INCLUDE = -I/usr/include/SFML -I/usr/local/include/Box2D -I/usr/include/boost

all: $(OBJS)
	$(CC) $(OBJS) $(C_FLAGS) $(L_FLAGS) $(INCLUDE) -o $(OBJ_NAME)
