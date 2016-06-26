#ifndef __RAYTRACING_H
#define __RAYTRACING_H

#include "objects.h"
#include <stdint.h>

typedef struct _thread_arg{
	uint8_t *pixels;
	double *background_color;
	rectangular_node rectangulars;
	sphere_node spheres;
	light_node lights;
	const viewpoint *view;
	int width;
	int height;
	int thnum;		
}thread_arg;

typedef struct _thr_arg{
	thread_arg *ptr;
	int start_height;
}thr_arg;


void *raytracing(void *arg1);
#endif
