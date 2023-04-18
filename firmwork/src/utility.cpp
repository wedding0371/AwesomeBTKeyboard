#include "utility.h"

int quadrant(float delta) {
    if (delta > 0 && delta <= 90)
        return 1;
    else if (delta > 90 && delta <= 180)
        return 2;
    else if (delta > -180 && delta <= -90)
        return 3;
    else
        return 4;
}