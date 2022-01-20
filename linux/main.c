// G4 Encoder demo
// Written by Larry Bank
// 
// Will create a 640x480 TIFF G4 image on the fly
//
#include <stdint.h>
#include <string.h>
#include <time.h>
#include "../src/G4Enc.h"
#include "../src/g4enc.inl"

G4ENCIMAGE g4;

long micros()
{
long iTime;
struct timespec res;

    clock_gettime(CLOCK_MONOTONIC, &res);
    iTime = 1000000*res.tv_sec + res.tv_nsec/1000;

    return iTime;
} /* micros() */

int main(int argc, char *argv[])
{
long lTime;
int rc;

    printf("G4 Encoder demo\n");

    printf("G4ENCIMAGE Structure size = %d bytes\n", (int)sizeof(G4ENCIMAGE));

    lTime = micros();
    lTime = micros() - lTime;
    printf("full sized decode in %d us\n", (int)lTime);

    return 0;
} /* main() */
