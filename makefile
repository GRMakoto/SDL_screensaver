build:
	gcc -Wall -Wpedantic ./src/*.c -lSDL2 -o game

wasm:
	emcc -O3 ./src/main.c -o game.js -s USE_SDL=2

run:
	./game

clean:
	rm game*
