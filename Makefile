BIN=cube
CFLAGS=-lSDL2 -lm

$(BIN): src/main.c src/cube_model.h src/matrix_multiplication.o
	gcc $(CFLAGS) -o $@ $^

%.o: %.asm
	nasm -felf64 $<

.PHONY: clean

clean:
	rm src/matrix_multiplication.o cube
