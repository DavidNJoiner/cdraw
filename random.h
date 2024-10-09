#ifndef RANDOM_H
#define RANDOM_H

#include <stdlib.h>
#include <stdio.h>
#include <time.h>


float frand(float max)
{
    float scale = (float)rand() / (float) RAND_MAX; /* [0, 1.0] */
    float value = scale * max;
    return value;        /* [min, max] */
}

#endif //RANDOM_H

