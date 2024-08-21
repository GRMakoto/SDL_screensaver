build:
	gcc -Wall -Wpedantic ./src/*.c -lSDL2 -o game

run:
	./game

clean:
	rm game
