MAKEFLAGS += --no-print-directory

all:
	cmake --build ./build --target marble -j 12 --

clean:
	cmake --build ./build --target clean

run:
	./build/src/Marble/marble -f main.mb

marble:
	./build/src/Marble/marble -f main.mb | clang -x ir - -o ./build/out
	./build/out

valgrind: 
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt ./build/src/Marble/marble -f main.mb


test:
	cmake --build ./build --target all -j 12 --
	ctest --test-dir build --output-on-failure