#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include "paralel_mandelbrot.h"

SDL_Surface* surface;
Mts scheduler;
struct timeval calcStart, calcEnd;

// Settings
const char* outputName = NULL;
int width = 640, height = 400;
unsigned int workers = 1;
unsigned int granularity = 1;
float x = -2.f,y = -2.f;
float x_end = 2.f,y_end = 2.f;
float scale_x = 1.f , scale_y = 1.f;
uint8_t quiet = 0;

int readParams(int argc, char** argv) {
  char* endPtr;
  for(unsigned int i = 1;i < argc; i++) {
    if (!strncmp(argv[i], "-r", 3) || !strncmp(argv[i], "-rect", 6)) {
      i++;
      if (i == argc)
        return 1;
      
      endPtr = argv[i];
      x     = strtod(endPtr, &endPtr);
      if(*endPtr != ':')
        return 1;
      endPtr++;
      x_end = strtod(endPtr, &endPtr);
      if(*endPtr != ':')
        return 1;
      endPtr++;
      y     = strtod(endPtr, &endPtr);
      if(*endPtr != ':')
        return 1;
      endPtr++;
      y_end = strtod(endPtr, &endPtr);
      if(*endPtr)
        return 1;
      continue;
    }
    if (!strncmp(argv[i], "-s", 3) || !strncmp(argv[i], "-size", 6)) {
      i++;
      if (i == argc)
        return 1;

      endPtr = argv[i];
      width  = strtoul(endPtr, &endPtr, 10);
      if(*endPtr != 'x')
        return 1;
      endPtr++;
      height = strtoul(endPtr, &endPtr, 10);
      if(*endPtr || width < 2 || height < 2)
        return 1;
      continue;
    }
    if (!strncmp(argv[i], "-t", 3) || !strncmp(argv[i], "-tasks", 7)) {
      i++;
      if (i == argc)
        return 1;

      workers = strtoul(argv[i], &endPtr, 10);
      if(*endPtr || workers < 1)
        return 1;
      continue;
    }
    if (!strncmp(argv[i], "-g", 3) || !strncmp(argv[i], "-granularity", 13)) {
      i++;
      if (i == argc)
        return 1;

      granularity = strtoul(argv[i], &endPtr, 10);
      if(*endPtr || granularity < 1)
        return 1;
      continue;
    }
    if (!strncmp(argv[i], "-q", 3) || !strncmp(argv[i], "-quiet", 7)) {
      quiet = 1;
      continue;
    }
    if (!strncmp(argv[i], "-o", 3) || !strncmp(argv[i], "-output", 8)) {
      i++;
      if (i == argc)
        return 1;

      outputName = argv[i];
      continue;
    }
    return 1;
  }
  return 0;
}

void usage(void) {
  printf("Usage:\n");
  printf("  mandelbrot [OPTIONS ....]\n");
  printf("OPTIONS:\n");
  printf(" -r or -rect followed by four floats x_start:x_end:y_start:y_end\n");
  printf(" -s or -size followed by two unsigned integers WIDTHxHEIGTH\n");
  printf(" -t or -tasks followed by one unsigned integer N\n");
  printf(" -g or -granularity followed by one unsigned integer N\n");
  printf(" -q or -quiet for quiet execution\n");
  printf(" -o or -output followed by string name\n");
  printf("    The name must be *.bmp because the program saves only bmp files.\n");
}

int main(int argc, char** argv) {
  if (readParams(argc, argv)) {
    usage();
    return 1;
  }
  surface = SDL_CreateRGBSurface(0, width, height, 32,
                                 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
  Mts_setTask(&scheduler, surface, granularity, x, y, (x_end - x)/(width-1),(y_end - y)/(height-1));
  if (!quiet) gettimeofday(&calcStart, NULL);
  Mts_startWorkers(&scheduler, workers);
  Mts_waitWorkers(&scheduler);
  if (!quiet) {
    gettimeofday(&calcEnd, NULL);
    printf("Thread used in current run: %u\n", workers);
    printf("Total execution time for current run (millis): %lf\n",
          (double) (calcEnd.tv_usec - calcStart.tv_usec) / 1000. +
          (double) (calcEnd.tv_sec - calcStart.tv_sec) * 1000.);
  }
  if (!outputName)
    outputName = "./zad20.bmp";
  if (SDL_SaveBMP(surface, outputName) && !quiet) {
    fprintf(stderr, "Error while saving the image\n");
    fprintf(stderr, "SDL2 Error:%s\n", SDL_GetError());
    exit(1);
  }
  SDL_FreeSurface(surface);
  return 0;
}
