build:
	gcc -Wall -Wpedantic -O0 ./src/*.c -lSDL2 -o game

run:
	./game

clean:
	rm game
