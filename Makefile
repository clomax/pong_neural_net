OBJS = src/pung.cpp
OBJ_NAME = build/pung

CC = clang++
C_FLAGS = -Wall -Wextra -std=c++11
B2D_FLAGS = -lBox2D
SFML_FLAGS = -lsfml-graphics -lsfml-window -lsfml-system

INCLUDE = -I/usr/include/SFML -I/usr/local/include/Box2D -I/usr/include/boost

D_CC = g++
DEBUG_OBJ_NAME = build/debug_pung
D_SFML_FLAGS = -lsfml-graphics -lsfml-window -lsfml-system -g

all: $(OBJS)
	$(CC) $(OBJS) $(C_FLAGS) $(SFML_FLAGS) $(B2D_FLAGS) $(INCLUDE) -o $(OBJ_NAME)

debug: $(OBJS)
	$(D_CC) $(OBJS) $(C_FLAGS) $(D_SFML_FLAGS) $(B2D_FLAGS) $(INCLUDE) -o $(DEBUG_OBJ_NAME)

slow: $(OBJS)
	$(CC) -DSLOW=1 $(OBJS) $(C_FLAGS) $(SFML_FLAGS) $(B2D_FLAGS) $(INCLUDE) -o $(OBJ_NAME)
