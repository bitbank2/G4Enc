#ifndef __G4ENC__
#define __G4ENC__
#if defined( __MACH__ ) || defined( __LINUX__ )
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#define memcpy_P memcpy
#define PROGMEM
#define pgm_read_byte(s) *s
#define pgm_read_word(s) *(int16_t *)s
#else
#include <Arduino.h>
#ifndef PROGMEM
#define PROGMEM
#define memcpy_P memcpy
#define pgm_read_byte(p) *(uint8_t *)p
#define pgm_read_word(p) *(uint16_t *)p
#endif
#endif
//
// CCITT G4 image encoding library
// Written by Larry Bank
// Copyright (c) 2022 BitBank Software, Inc.
// 
// Designed to encode 1-bpp images
// as highly compressed CCITT G4 data
//

/* Defines and variables */
#define OUTPUT_BUF_SIZE 1024
#define G4ENC_MAX_WIDTH 1024
#define G4ENC_MSB_FIRST     1
#define G4ENC_LSB_FIRST     2

// Error codes returned by getLastError()
enum {
    G4ENC_SUCCESS = 0,
    G4ENC_NOT_INITIALIZED,
    G4ENC_INVALID_PARAMETER,
    G4ENC_DATA_OVERFLOW,
    G4ENC_IMAGE_COMPLETE
};

typedef struct pil_buffered_bits
{
unsigned char *pBuf; // buffer pointer
uint32_t ulBits; // buffered bits
uint32_t ulBitOff; // current bit offset
uint32_t ulDataSize; // available data
} BUFFERED_BITS;

typedef int (G4ENC_WRITE_CALLBACK)(uint8_t *pBuf, int iLen);

//
// our private structure to hold a TIFF image decode state
//
typedef struct g4enc_image_tag
{
    int iWidth, iHeight; // image size
    int iError;
    int y; // last y encoded
    uint8_t ucFillOrder;
    int iOutSize;
    int iDataSize; // generated output size
    uint8_t *pOutBuf;
    int16_t *pCur, *pRef; // pointers to swap current and reference lines
    G4ENC_WRITE_CALLBACK *pfnWrite;
    BUFFERED_BITS bb;
    int16_t CurFlips[G4ENC_MAX_WIDTH];
    int16_t RefFlips[G4ENC_MAX_WIDTH];
    uint8_t ucFileBuf[OUTPUT_BUF_SIZE]; // holds temporary output data
} G4ENCIMAGE;

#ifdef __cplusplus
//
// The G4Enc class wraps portable C code which does the actual work
//
class G4ENC
{
  public:
    int init(int iWidth, int iHeight, int iBitDirection, G4ENC_WRITE_CALLBACK *pfnWrite, uint8_t *pOut, int iOutSize);
    int getTIFFHeaderSize();
    int getTIFFHeader(uint8_t *pOut);
    int addLine(uint8_t *pPixels);
    int getOutSize();

  private:
    G4ENCIMAGE _g4;
};
#else
int G4ENC_init(G4ENCIMAGE *pImage, int iWidth, int iHeight, int iBitDirection, G4ENC_WRITE_CALLBACK *pfnWrite, uint8_t *pOut, int iOutSize);
int G4ENC_getTIFFHeaderSize(G4ENCIMAGE *pImage);
int G4ENC_getTIFFHeader(G4ENCIMAGE *pImage, uint8_t *pOut);
int G4ENC_addLine(G4ENCIMAGE *pImage, uint8_t *pPixels);
int G4ENC_getOutSize(G4ENCIMAGE *pImage);
#endif

// Due to unaligned memory causing an exception, we have to do these macros the slow way
#define MOTOLONG(p) (((*p)<<24UL) + ((*(p+1))<<16UL) + ((*(p+2))<<8UL) + (*(p+3)))
#define TOP_BIT 0x80000000
#define MAX_VALUE 0xffffffff
#define BIGUINT uint32_t
#define LONGWHITECODEMASK 0x2000000
#define LONGBLACKCODEMASK 0x10000000
// Must be a 32-bit target processor
#define REGISTER_WIDTH 32

#endif // __G4ENC__
