// G4 Encoder demo
// Written by Larry Bank
// 
// Compresses a given Windows BMP fill into T.6 (CCITT G4) data
// It can output a block of raw compressed data or if the name given
// in the second parameter ends in ".tif", it will add a TIFF header
//
// Copyright 2022 BitBank Software, Inc. All Rights Reserved.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//    http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//===========================================================================
//
#include <stdint.h>
#include <string.h>
#include <time.h>
#include "../src/G4ENCODER.h"
#include "../src/g4enc.inl"

G4ENCIMAGE g4;

long micros(void)
{
long iTime;
struct timespec res;

    clock_gettime(CLOCK_MONOTONIC, &res);
    iTime = 1000000*res.tv_sec + res.tv_nsec/1000;

    return iTime;
} /* micros() */

//
// Read a Windows BMP file into memory
// For this demo, the only supported files are 24 or 32-bits per pixel
//
uint8_t * ReadBMP(const char *fname, int *width, int *height, int *bpp, unsigned char *pPal)
{
    int y, w, h, bits, offset;
    uint8_t *s, *d, *pTemp, *pBitmap;
    int pitch, bytewidth;
    int iSize, iDelta;
    FILE *infile;

    infile = fopen(fname, "r+b");
    if (infile == NULL) {
        printf("Error opening input file %s\n", fname);
        return NULL;
    }
    // Read the bitmap into RAM
    fseek(infile, 0, SEEK_END);
    iSize = (int)ftell(infile);
    fseek(infile, 0, SEEK_SET);
    pBitmap = (uint8_t *)malloc(iSize);
    pTemp = (uint8_t *)malloc(iSize);
    fread(pTemp, 1, iSize, infile);
    fclose(infile);

    if (pTemp[0] != 'B' || pTemp[1] != 'M' || pTemp[14] < 0x28) {
        free(pBitmap);
        free(pTemp);
        printf("Not a Windows BMP file!\n");
        return NULL;
    }
    w = *(int32_t *)&pTemp[18];
    h = *(int32_t *)&pTemp[22];
    bits = *(int16_t *)&pTemp[26] * *(int16_t *)&pTemp[28];
    if (bits <= 8) { // it has a palette, copy it
        uint8_t *p = pPal;
        for (int i=0; i<(1<<bits); i++)
        {
           *p++ = pTemp[54+i*4];
           *p++ = pTemp[55+i*4];
           *p++ = pTemp[56+i*4];
        }
    }
    offset = *(int32_t *)&pTemp[10]; // offset to bits
    bytewidth = ((w * bits)+7) >> 3;
    pitch = (bytewidth + 3) & 0xfffc; // DWORD aligned
// move up the pixels
    d = pBitmap;
    s = &pTemp[offset];
    iDelta = pitch;
    if (h > 0) {
        iDelta = -pitch;
        s = &pTemp[offset + (h-1) * pitch];
    } else {
        h = -h;
    }
    for (y=0; y<h; y++) {
        if (bits == 32) {// need to swap red and blue
            for (int i=0; i<bytewidth; i+=4) {
                d[i] = s[i+2];
                d[i+1] = s[i+1];
                d[i+2] = s[i];
                d[i+3] = s[i+3];
            }
        } else {
            memcpy(d, s, bytewidth);
        }
        d += bytewidth;
        s += iDelta;
    }
    *width = w;
    *height = h;
    *bpp = bits;
    free(pTemp);
    return pBitmap;

} /* ReadBMP() */

int main(int argc, char *argv[])
{
long lTime;
int rc;
uint8_t *pTemp;
uint8_t *pBitmap;
int iSize, iWidth, iHeight, iBpp, iPitch;
uint8_t ucPalette[1024];
FILE *oHandle;
    
    printf("G4 Encoder demo\n");
    printf("G4ENCIMAGE Structure size = %d bytes\n", (int)sizeof(G4ENCIMAGE));

    if (argc != 3) {
        printf("Usage: g4demo <infile> <outfile>\n");
        printf("The input file should be a 1-bpp Windows BMP file\n");
        printf("The output file will be a TIFF file if the name ends in .tif,\n");
        printf("otherwise it will be just the compressed image data.\n");
        return 0;
    }
    pBitmap = ReadBMP(argv[1], &iWidth, &iHeight, &iBpp, ucPalette);
    if (iBpp != 1) {
        printf("Input image must be 1-bpp\n");
        return 0;
    }
    if (pBitmap != NULL) {
        iPitch = (iWidth+7)>>3;
        iSize = iPitch * iHeight; // allocate enough to hold an uncompressed copy
        pTemp = (uint8_t *)malloc(iSize);
        lTime = micros();
        rc = G4ENC_init(&g4, iWidth, iHeight, G4ENC_MSB_FIRST, NULL, pTemp, iSize);
        if (rc == G4ENC_SUCCESS) {
            for (int i=0; i<iHeight && rc == G4ENC_SUCCESS; i++) {
                rc = G4ENC_addLine(&g4, &pBitmap[i * iPitch]);
            }
        }
        lTime = micros() - lTime;
        printf("Encode in %d us\n", (int)lTime);
        printf("Output data size = %d bytes\n", G4ENC_getOutSize(&g4));
        oHandle = fopen(argv[2], "w+b");
        if (oHandle == NULL) {
            printf("Error opening output file %s\n", argv[2]);
            return 0;
        }
        if (memcmp(&argv[2][strlen(argv[2])-4], ".tif", 4) == 0) {
            // output file is requested to be a TIFF, write the header first
            printf("Output file requested to be a TIFF; adding header...\n");
            uint8_t ucTemp[256];
            G4ENC_getTIFFHeader(&g4, ucTemp);
            iSize = G4ENC_getTIFFHeaderSize();
            fwrite(ucTemp, 1, iSize, oHandle);
        }
        fwrite(pTemp, 1, G4ENC_getOutSize(&g4), oHandle);
        fclose(oHandle);
    }
    return 0;
} /* main() */
