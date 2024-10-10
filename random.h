#ifndef RANDOM_H
#define RANDOM_H

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>


float frand(float max)
{
    float scale = (float)rand() / (float) RAND_MAX; /* [0, 1.0] */
    float value = scale * max;
    return value;        /* [min, max] */
}

float frand_clustered(float max, float power)
{
    float scale = (float)rand() / (float) RAND_MAX; /* [0, 1.0] */
    float value = powf(scale, power) * max;
    return value;        /* [0, max] */
}

#endif //RANDOM_H

