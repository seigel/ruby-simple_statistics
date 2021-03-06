#ifndef _SSTAT_TYPE_DEF_H_
#define _SSTAT_TYPE_DEF_H_

#include <stdio.h>

/* Define constants and structures used for survival analysis functions */

/* EPSILON here is set based on Ruby float. */
#define EPSILON 2.2204460492503131e-16

/* following constants are from ATMEL http://www.atmel.com/webdoc/ */
#define M_2_SQRTPI 1.12837916709551257390
#define M_SQRT1_2 0.70710678118654752440
#define M_1_SQRT2PI (M_2_SQRTPI * M_SQRT1_2 / 2.0)
#define M_SQRT2 1.41421356237309504880
#define SQRT32 (4.0 * M_SQRT2)

/* Constants for gaussian distribution */
#define GAUSS_XUPPER (8.572)
#define GAUSS_XLOWER (-37)
#define GAUSS_SCALE (16.0)
#define GAUSS_EPSILON 1e-12

typedef struct point
{
    double x;
    double y;
} point;

typedef struct curve
{
    struct point* point_array;
    int size;
} curve;

typedef struct array
{
    double* D_ptr;
    int size;
} array;


point* alloc_points(int size) {
    return (struct point*) malloc(size * sizeof(struct point));
}

void print_points(struct point* points, int size)
{
    int i;
    for(i = 0; i < size; i++)
    {
        printf("point %i, x : %f, y : %f \n", i, points[i].x, points[i].y);
    }
}

void print_curve(struct curve* curve)
{
    int i;
    for(i = 0; i < curve->size; i++)
    {
        printf("x: %f  y: %f\n", curve->point_array[i].x, curve->point_array[i].y);
    }

}

#endif