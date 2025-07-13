MAKEFLAGS += --no-print-directory

all:
	cmake --build ./build --target marble -j 9 --

clean:
	cmake --build ./build --target clean

run:
	./build/src/Marble/marble -f main.mb

marble:
	./build/src/Marble/marble -f main.mb | clang -x ir - -o ./build/out
	./build/out

test:
	cmake --build ./build --target all -j 9 --
	ctest --test-dir build --output-on-failure