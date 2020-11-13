all: main.o paralel_mandelbrot.o paralel_mandelbrot.h 
	gcc main.o paralel_mandelbrot.o -Wall -o mandelbrot -lSDL2 -lm -pthread

main.o: main.c paralel_mandelbrot.h
	gcc -c main.c -Wall -o main.o

paralel_mandelbrot.o: paralel_mandelbrot.c paralel_mandelbrot.h
	gcc -c paralel_mandelbrot.c -Wall -o paralel_mandelbrot.o

build_static: main.o paralel_mandelbrot.o paralel_mandelbrot.h
	gcc main.o paralel_mandelbrot.o -Wall -o mandelbrot -l:libSDL2.a -lm -ldl -lpthread -lrt

build_optimised: main.o_optimise paralel_mandelbrot.o_optimise

main.o_optimise: main.c paralel_mandelbrot.h
	gcc -O3 -c main.c -o main.o

paralel_mandelbrot.o_optimise: paralel_mandelbrot.c paralel_mandelbrot.h
	gcc -O3 -c paralel_mandelbrot.c -o paralel_mandelbrot.o

clean:
	rm -vf main.o paralel_mandelbrot.o mandelbrot

.PHONY: clean build_optimised main.o_optimise paralel_mandelbrot.o_optimise build_static
