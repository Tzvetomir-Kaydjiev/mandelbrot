#include <stdlib.h>
#include <string.h>
#include <complex.h>
#include <sys/time.h>
#include "paralel_mandelbrot.h"

uint8_t mandelbrotIterations(float x, float y) {
  uint8_t steps = 0;
  float complex point = x + y * I;
  float complex jupingPoint = point;
  while (crealf(jupingPoint) * crealf(jupingPoint) + cimagf(jupingPoint) * cimagf(jupingPoint) < 4 && steps < 255) {
    jupingPoint = jupingPoint * jupingPoint;
    jupingPoint = jupingPoint * cexpf(jupingPoint) + point;
    steps++;
  }
  return 255 - steps;
}

void renderLine(SDL_Surface* surface, unsigned int lineNum, float x, float y, float scale_x, float scale_y) {
  uint8_t* pixels = surface->pixels;
  for(int i = 0; i < surface->w; i++) {
    pixels[lineNum * surface->pitch + i * 4 + 0] = mandelbrotIterations(x + i * scale_x, y + lineNum * scale_y);
    pixels[lineNum * surface->pitch + i * 4 + 1] = pixels[lineNum * surface->pitch + i * 4 + 0];
    pixels[lineNum * surface->pitch + i * 4 + 2] = pixels[lineNum * surface->pitch + i * 4 + 0];
    pixels[lineNum * surface->pitch + i * 4 + 3] = 255;
  }
}

struct MwfParams {
  Mts* task;
  size_t thread_num;
};

struct MwfParams* params;
extern uint8_t quiet;

void* mwf(void * _args) {
  struct timeval calcStart, calcEnd;
  struct MwfParams* params = _args;
  Mts* task = params->task;
  unsigned int currentTask;
  if (!quiet) {
    printf("Thread - %ld started.\n", params->thread_num);
    gettimeofday(&calcStart, NULL);
  }
  while (1) {
    pthread_mutex_lock(&task->lock);
    currentTask = task->progress;
    task->progress += task->granularity;
    pthread_mutex_unlock(&task->lock);
    for(unsigned i = 0; i < task->granularity; i++) {
      if (currentTask < task->surface->h) {
        renderLine(task->surface, currentTask, task->x, task->y, task->scale_x, task->scale_y);
      } else {
        goto finished;
      }
      currentTask++;
    }
  }
  finished:
  if (!quiet) {
  gettimeofday(&calcEnd, NULL);
  printf("Thread - %ld stopped.\n", params->thread_num);
  printf("Thread - %ld execution time was (millis): %lf\n", params->thread_num,
        (double) (calcEnd.tv_usec - calcStart.tv_usec) / 1000. +
        (double) (calcEnd.tv_sec - calcStart.tv_sec) * 1000.);
  }
  return NULL;
}

void Mts_setTask(Mts* self, SDL_Surface* surface, unsigned int granularity, float x, float y, float scale_x, float scale_y) {
  self->surface = surface;
  self->progress = 0;
  self->granularity = granularity;
  self->x = x;
  self->y = y;
  self->scale_x = scale_x;
  self->scale_y = scale_y;
}

void Mts_startWorkers(Mts* self, unsigned int count) {
  params = malloc(sizeof(struct MwfParams) * count);
  memset(params, 0, sizeof(struct MwfParams) * count);
  self->workerCount = count;
  self->workerList = malloc(sizeof(pthread_t) * count);
  memset(self->workerList, 0, count * sizeof(pthread_t));
  for (unsigned int i = 0; i < count; i++) {
    params[i].thread_num = i;
    params[i].task = self;
    pthread_create(self->workerList + i, NULL, mwf, params + i);
  }
}
  
void Mts_waitWorkers(Mts* self) {
  for (unsigned int i = 0; i < self->workerCount; i++)
    pthread_join(self->workerList[i], NULL);
  free(self->workerList);
  pthread_mutex_destroy(&self->lock);
  self->workerCount = 0;
  self->workerList = NULL;
  free(params);
  params = NULL;
}