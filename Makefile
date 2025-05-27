MAKEFLAGS += --no-print-directory

all:
	cmake --build ./build --target all -j 18 --

clean:
	cmake --build ./build --target clean

run:
	./build/src/Marble/marble -f main.mb