#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>

#include "primitives.h"
#include "raytracing.h"

#define OUT_FILENAME "out.ppm"

#define ROWS 512
#define COLS 512
#define NUM_THREADS 4

static void write_to_ppm(FILE *outfile, uint8_t *pixels,
                         int width, int height)
{
    fprintf(outfile, "P6\n%d %d\n%d\n", width, height, 255);
    fwrite(pixels, 1, height * width * 3, outfile);
}

static double diff_in_second(struct timespec t1, struct timespec t2)
{
    struct timespec diff;
    if (t2.tv_nsec-t1.tv_nsec < 0) {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec - 1;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec + 1000000000;
    } else {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec;
    }
    return (diff.tv_sec + diff.tv_nsec / 1000000000.0);
}

int main()
    {
        uint8_t *pixels;
        light_node lights = NULL;
        rectangular_node rectangulars = NULL;
        sphere_node spheres = NULL;
        color background_color = { 0.0, 0.1, 0.1 };
        struct timespec start, end;

#include "use-models.h"

        /* allocate by the given resolution */
        pixels = malloc(sizeof(unsigned char) * ROWS * COLS * 3);
        if (!pixels) exit(-1);
        
        pthread_t thr[NUM_THREADS];
        int i,rc;
        thr_arg *arg[NUM_THREADS];
	thread_arg *th_ptr;
 	void *ret;
	        
        th_ptr=(thread_arg *)malloc(sizeof(thread_arg));

        th_ptr->pixels = pixels;
        th_ptr->background_color = background_color;
        th_ptr->rectangulars = rectangulars;
        th_ptr->spheres = spheres;
        th_ptr->lights = lights;
        th_ptr->view = &view;
        th_ptr->width = COLS;
        th_ptr->height = ROWS;
	th_ptr->thnum = NUM_THREADS;


        for(i=0;i<NUM_THREADS;++i){
            arg[i]=(thr_arg *)malloc(sizeof(thr_arg));
            arg[i]->start_height = i;
            arg[i]->ptr = th_ptr;
         }
       /* do the ray tracing with the given geometry */
       clock_gettime(CLOCK_REALTIME, &start);
        for(i=0;i<NUM_THREADS;++i){
            if((rc=pthread_create(&thr[i],NULL,raytracing,arg[i]))!=0){
               printf("thread create fail \n");
               exit(1);	
             }
           }
        for(i=0;i<NUM_THREADS;++i){
        pthread_join(thr[i],&ret);
           }
        
        clock_gettime(CLOCK_REALTIME, &end);
        printf("# Rendering scene\n");

        {
            FILE *outfile = fopen(OUT_FILENAME, "wb");
            write_to_ppm(outfile, pixels, ROWS, COLS);
            fclose(outfile);
        }

        delete_rectangular_list(&rectangulars);
        delete_sphere_list(&spheres);
        delete_light_list(&lights);
        free(pixels);
        free(th_ptr);
        printf("Done!\n");
        printf("Execution time of raytracing() : %lf sec\n", diff_in_second(start, end));
        return 0;
    }
