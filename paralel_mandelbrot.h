#ifndef _PARALEL_MANDELBROT_H_
#define _PARALEL_MANDELBROT_H_
#include <SDL2/SDL.h>
#include <pthread.h>
#include <stdint.h>

struct mandelbrotTaskSheduler {
  pthread_mutex_t lock;
  pthread_t *workerList;
  SDL_Surface *surface;
  unsigned int progress;
  unsigned int workerCount;
  //unsigned int idleWorkers;
  unsigned int granularity;
  float x, y, scale_x, scale_y;
  unsigned char stop;
};
typedef struct mandelbrotTaskSheduler Mts;

void Mts_setTask(Mts* self, SDL_Surface* surface, unsigned int granularity, float x, float y, float scale_x, float scale_y);
void Mts_startWorkers(Mts* self, unsigned int count);
void Mts_waitWorkers(Mts* self);
#endif /*_PARALEL_MANDELBROT_H_*/
