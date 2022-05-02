INCLUDES= -I ./include
FLAGS= -g 

OBJECTS= ./build/chip8memory.o ./build/chip8stack.o ./build/chip8keyboard.o ./build/chip8.o ./build/chip8screen.o

all: ${OBJECTS}
	gcc ${FLAGS} ${INCLUDES} ./source/main.c ${OBJECTS} -L ./lib -lmingw32 -lSDL2main -lSDL2 -o ./bin/main

build/chip8memory.o: source/chip8memory.c
	gcc ${FLAGS}  ${INCLUDES} ./source/chip8memory.c -c -o ./build/chip8memory.o

build/chip8stack.o: source/chip8stack.c
	gcc ${FLAGS}  ${INCLUDES} ./source/chip8stack.c -c -o ./build/chip8stack.o

build/chip8keyboard.o: source/chip8keyboard.c
	gcc ${FLAGS}  ${INCLUDES} ./source/chip8keyboard.c -c -o ./build/chip8keyboard.o

build/chip8.o: source/chip8.c
	gcc ${FLAGS}  ${INCLUDES} ./source/chip8.c -c -o ./build/chip8.o

build/chip8screen.o: source/chip8screen.c
	gcc ${FLAGS}  ${INCLUDES} ./source/chip8screen.c -c -o ./build/chip8screen.o

clean: 
	del build\*