//
// G4Enc
// A CCITT G4 / 1-bpp image encoding library
// written by Larry Bank
// bitbank@pobox.com
// Arduino port started 1/19/2022
// Original code written more than 20 years ago :)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#include "G4ENCODER.h"

/* Number of consecutive 1 bits in a byte from MSB to LSB */
static uint8_t bitcount[256] =
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  /* 0-15 */
         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  /* 16-31 */
         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  /* 32-47 */
         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  /* 48-63 */
         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  /* 64-79 */
         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  /* 80-95 */
         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  /* 96-111 */
         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  /* 112-127 */
         1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  /* 128-143 */
         1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  /* 144-159 */
         1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  /* 160-175 */
         1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  /* 176-191 */
         2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,  /* 192-207 */
         2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,  /* 208-223 */
         3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,  /* 224-239 */
         4,4,4,4,4,4,4,4,5,5,5,5,6,6,7,8}; /* 240-255 */

/* Table of vertical codes for G4 encoding */
/* code followed by length, starting with v(-3) */
static const uint8_t vtable[14] =
        {3,7,     /* V(-3) = 0000011 */
         3,6,     /* V(-2) = 000011  */
         3,3,     /* V(-1) = 011     */
         1,1,     /* V(0)  = 1       */
         2,3,     /* V(1)  = 010     */
         2,6,     /* V(2)  = 000010  */
         2,7};    /* V(3)  = 0000010 */

/* Group 3 Huffman codes ordered for MH encoding */
/* first, the terminating codes for white (code, length) */
static const short huff_white[128] =
        {0x35,8,7,6,7,4,8,4,0xb,4, /* 0,1,2,3,4 */
         0xc,4,0xe,4,0xf,4,0x13,5,0x14,5,7,5,8,5, /* 5,6,7,8,9,10,11 */
         8,6,3,6,0x34,6,0x35,6,0x2a,6,0x2b,6,0x27,7, /* 12,13,14,15,16,17,18 */
         0xc,7,8,7,0x17,7,3,7,4,7,0x28,7,0x2b,7, /* 19,20,21,22,23,24,25 */
         0x13,7,0x24,7,0x18,7,2,8,3,8,0x1a,8,0x1b,8, /* 26,27,28,29,30,31,32 */
         0x12,8,0x13,8,0x14,8,0x15,8,0x16,8,0x17,8,0x28,8, /* 33,34,35,36,37,38,39 */
         0x29,8,0x2a,8,0x2b,8,0x2c,8,0x2d,8,4,8,5,8, /* 40,41,42,43,44,45,46 */
         0xa,8,0xb,8,0x52,8,0x53,8,0x54,8,0x55,8,0x24,8, /* 47,48,49,50,51,52,53 */
         0x25,8,0x58,8,0x59,8,0x5a,8,0x5b,8,0x4a,8,0x4b,8, /* 54,55,56,57,58,59,60 */
         0x32,8,0x33,8,0x34,8};                        /* 61,62,63 */

/* now the white make-up codes */
static const short huff_wmuc[82] =
       {0,0,0x1b,5,0x12,5,0x17,6,0x37,7,0x36,8,   /* null,64,128,192,256,320 */
        0x37,8,0x64,8,0x65,8,0x68,8,0x67,8,0xcc,9, /* 384,448,512,576,640,704 */
        0xcd,9,0xd2,9,0xd3,9,0xd4,9,0xd5,9,    /* 768,832,896,960,1024 */
        0xd6,9,0xd7,9,0xd8,9,0xd9,9,0xda,9,    /* 1088,1152,1216,1280,1344 */
        0xdb,9,0x98,9,0x99,9,0x9a,9,0x18,6,    /* 1408,1472,1536,1600,1664 */
        0x9b,9,8,11,0xc,11,0xd,11,0x12,12,     /* 1728,1792,1856,1920,1984 */
        0x13,12,0x14,12,0x15,12,0x16,12,0x17,12, /* 2048,2112,2176,2240,2304 */
        0x1c,12,0x1d,12,0x1e,12,0x1f,12};       /* 2368,2432,2496,2560 */

/* black terminating codes */
static const short huff_black[128] =
      {0x37,10,2,3,3,2,2,2,3,3,                         /* 0,1,2,3,4 */
       3,4,2,4,3,5,5,6,4,6,4,7,5,7,                     /* 5,6,7,8,9,10,11 */
       7,7,4,8,7,8,0x18,9,0x17,10,0x18,10,8,10,         /* 12,13,14,15,16,17,18 */
       0x67,11,0x68,11,0x6c,11,0x37,11,0x28,11,0x17,11, /* 19,20,21,22,23,24 */
       0x18,11,0xca,12,0xcb,12,0xcc,12,0xcd,12,0x68,12, /* 25,26,27,28,29,30 */
       0x69,12,0x6a,12,0x6b,12,0xd2,12,0xd3,12,0xd4,12, /* 31,32,33,34,35,36 */
       0xd5,12,0xd6,12,0xd7,12,0x6c,12,0x6d,12,0xda,12, /* 37,38,39,40,41,42 */
       0xdb,12,0x54,12,0x55,12,0x56,12,0x57,12,0x64,12, /* 43,44,45,46,47,48 */
       0x65,12,0x52,12,0x53,12,0x24,12,0x37,12,0x38,12, /* 49,50,51,52,53,54 */
       0x27,12,0x28,12,0x58,12,0x59,12,0x2b,12,0x2c,12, /* 55,56,57,58,59,60 */
       0x5a,12,0x66,12,0x67,12};                        /* 61,62,63 */
/* black make up codes */
static const short huff_bmuc[82] =
       {0,0,0xf,10,0xc8,12,0xc9,12,0x5b,12,0x33,12, /* null,64,128,192,256,320 */
        0x34,12,0x35,12,0x6c,13,0x6d,13,0x4a,13,0x4b,13,   /* 384,448,512,576,640,704 */
        0x4c,13,0x4d,13,0x72,13,0x73,13,0x74,13,0x75,13,   /* 768,832,896,960,1024,1088 */
        0x76,13,0x77,13,0x52,13,0x53,13,0x54,13,0x55,13,   /* 1152,1216,1280,1344,1408,1472 */
        0x5a,13,0x5b,13,0x64,13,0x65,13,8,11,0xc,11,       /* 1536,1600,1664,1728,1792,1856 */
        0xd,11,0x12,12,0x13,12,0x14,12,0x15,12,0x16,12,    /* 1920,1984,2048,2112,2176,2240 */
        0x17,12,0x1c,12,0x1d,12,0x1e,12,0x1f,12};          /* 2304,2368,2432,2496,2560 */

/* Table of byte flip values to mirror-image incoming CCITT data */
static const uint8_t ucMirror[256] =
     {0, 128, 64, 192, 32, 160, 96, 224, 16, 144, 80, 208, 48, 176, 112, 240,
      8, 136, 72, 200, 40, 168, 104, 232, 24, 152, 88, 216, 56, 184, 120, 248,
      4, 132, 68, 196, 36, 164, 100, 228, 20, 148, 84, 212, 52, 180, 116, 244,
      12, 140, 76, 204, 44, 172, 108, 236, 28, 156, 92, 220, 60, 188, 124, 252,
      2, 130, 66, 194, 34, 162, 98, 226, 18, 146, 82, 210, 50, 178, 114, 242,
      10, 138, 74, 202, 42, 170, 106, 234, 26, 154, 90, 218, 58, 186, 122, 250,
      6, 134, 70, 198, 38, 166, 102, 230, 22, 150, 86, 214, 54, 182, 118, 246,
      14, 142, 78, 206, 46, 174, 110, 238, 30, 158, 94, 222, 62, 190, 126, 254,
      1, 129, 65, 193, 33, 161, 97, 225, 17, 145, 81, 209, 49, 177, 113, 241,
      9, 137, 73, 201, 41, 169, 105, 233, 25, 153, 89, 217, 57, 185, 121, 249,
      5, 133, 69, 197, 37, 165, 101, 229, 21, 149, 85, 213, 53, 181, 117, 245,
      13, 141, 77, 205, 45, 173, 109, 237, 29, 157, 93, 221, 61, 189, 125, 253,
      3, 131, 67, 195, 35, 163, 99, 227, 19, 147, 83, 211, 51, 179, 115, 243,
      11, 139, 75, 203, 43, 171, 107, 235, 27, 155, 91, 219, 59, 187, 123, 251,
      7, 135, 71, 199, 39, 167, 103, 231, 23, 151, 87, 215, 55, 183, 119, 247,
      15, 143, 79, 207, 47, 175, 111, 239, 31, 159, 95, 223, 63, 191, 127, 255};

const char *SOFTWARE = (char *)"Created with G4ENCODER by Larry Bank";

static void G4ENCInsertCode(BUFFERED_BITS *bb, BIGUINT ulCode, int iLen)
{
    if ((bb->ulBitOff + iLen) > REGISTER_WIDTH) { // need to write data
        bb->ulBits |= (ulCode >> (bb->ulBitOff + iLen - REGISTER_WIDTH)); // partial bits on first word
        *(BIGUINT *)bb->pBuf = __builtin_bswap32(bb->ulBits);
        bb->pBuf += sizeof(BIGUINT);
        bb->ulBits = ulCode << ((REGISTER_WIDTH*2) - (bb->ulBitOff + iLen));
        bb->ulBitOff += iLen - REGISTER_WIDTH;
    } else {
        bb->ulBits |= (ulCode << (REGISTER_WIDTH - bb->ulBitOff - iLen));
        bb->ulBitOff += iLen;
    }
} /* G4ENCInsertCode() */
//
// Flush any buffered bits to the output
//
void G4ENCFlushBits(BUFFERED_BITS *bb)
{
    while (bb->ulBitOff >= 8)
    {
        *bb->pBuf++ = (unsigned char) (bb->ulBits >> (REGISTER_WIDTH - 8));
        bb->ulBits <<= 8;
        bb->ulBitOff -= 8;
    }
   *bb->pBuf++ = (unsigned char) (bb->ulBits >> (REGISTER_WIDTH - 8));
   bb->ulBitOff = 0;
   bb->ulBits = 0;
} /* G4ENCFlushBits() */
//
// Internal function to add a WHITE pixel run
//
void G4ENCAddWhite(int iLen, BUFFERED_BITS *bb)
{
while (iLen >= 64)
   {
       if (iLen >= 2560)
    {
        G4ENCInsertCode(bb, 0x1f, 12); /* Add the 2560 code */
        iLen -= 2560;
    }
    else
      {
      int iCode;
      iCode = iLen >> 6; /* Makeup code = mult of 64 */
          G4ENCInsertCode(bb, huff_wmuc[iCode*2], huff_wmuc[iCode*2+1]);
      iLen &= 63; /* Get the remainder */
      }
   }
   /* Add the terminating code */
    G4ENCInsertCode(bb, huff_white[iLen*2], huff_white[iLen*2+1]);
} /* G4ENCAddWhite() */
//
// Internal function to add a BLACK pixel run
//
static void G4ENCAddBlack(int iLen, BUFFERED_BITS *bb)
{
while (iLen >= 64)
   {
   if (iLen >= 2560)
      {
          G4ENCInsertCode(bb, 0x1f, 12); /* Add the 2560 code */
      iLen -= 2560;
      }
   else
      {
      int iCode;
      iCode = iLen >> 6; /* Makeup code = mult of 64 */
          G4ENCInsertCode(bb, huff_bmuc[iCode*2], huff_bmuc[iCode*2+1]);
      iLen &= 63; /* Get the remainder */
      }
   }
   /* Add the terminating code */
    G4ENCInsertCode(bb, huff_black[iLen*2], huff_black[iLen*2+1]);
} /* PILAddBlack() */
//
// Initialize the compressor
// This must be called before adding data to the output
//
int G4ENC_init(G4ENCIMAGE *pImage, int iWidth, int iHeight, int iBitDirection, G4ENC_WRITE_CALLBACK *pfnWrite, uint8_t *pOut, int iOutSize)
{
    int iError = G4ENC_SUCCESS;
    
    if (pImage == NULL || iWidth > G4ENC_MAX_WIDTH || iHeight <= 0 || (iBitDirection != G4ENC_LSB_FIRST && iBitDirection != G4ENC_MSB_FIRST))
        return G4ENC_INVALID_PARAMETER;
    pImage->iWidth = iWidth; // image size
    pImage->iHeight = iHeight;
    pImage->pCur = pImage->CurFlips;
    pImage->pRef = pImage->RefFlips;
    pImage->ucFillOrder = (uint8_t)iBitDirection;
    pImage->pfnWrite = pfnWrite; // optional output write callback
    pImage->pOutBuf = pOut; // optional output buffer
    pImage->iOutSize = iOutSize; // output buffer pre-allocated size
    pImage->iDataSize = 0; // no data yet
    pImage->y = 0;
    for (int i=0; i<G4ENC_MAX_WIDTH; i++) {
        pImage->RefFlips[i] = iWidth;
        pImage->CurFlips[i] = iWidth;
    }
    pImage->bb.pBuf = pImage->ucFileBuf;
    pImage->bb.ulBits = 0;
    pImage->bb.ulBitOff = 0;
    pImage->iError = iError;
    return iError;
} /* G4ENC_init() */
//
// Internal function to convert uncompressed 1-bit per pixel data
// into the run-end data needed to feed the G4 encoder
//
static void G4ENCEncodeLine(unsigned char *buf, int xsize, int16_t *pDest)
{
int iCount, xborder;
uint8_t i, c;
int8_t cBits;
int iLen;
int16_t x;

   xborder = xsize;
   iCount = (xsize + 7) >> 3; /* Number of bytes per line */
   cBits = 8;
   iLen = 0; /* Current run length */
   x = 0;

   c = *buf++;  /* Get the first byte to start */
   iCount--;
   while (iCount >=0)
      {
      i = bitcount[c]; /* Get the number of consecutive bits */
      iLen += i; /* Add this length to total run length */
      c <<= i;
      cBits -= i; /* Minus the number in a byte */
      if (cBits <= 0)
         {
         iLen += cBits; /* Adjust length */
         cBits = 8;
         c = *buf++;  /* Get another data byte */
         iCount--;
         continue; /* Keep doing white until color change */
         }
      c = ~c; /* flip color to count black pixels */
   /* Store the white run length */
      xborder -= iLen;
      if (xborder < 0)
         {
         iLen += xborder; /* Make sure run length is not past end */
         break;
         }
      x += iLen;
      *pDest++ = x;
      iLen = 0;
doblack:
      i = bitcount[c]; /* Get consecutive bits */
      iLen += i; /* Add to total run length */
      c <<= i;
      cBits -= i;
      if (cBits <= 0)
         {
         iLen += cBits; /* Adjust length */
         cBits = 8;
         c = *buf++;  /* Get another data byte */
         c = ~c;   /* Flip color to find black */
         iCount--;
         if (iCount < 0)
            break;
         goto doblack;
         }
   /* Store the black run length */
      c = ~c;       /* Flip color again to find white pixels */
      xborder -= iLen;
      if (xborder < 0)
         {
         iLen += xborder; /* Make sure run length is not past end */
         break;
         }
      x += iLen;
      *pDest++ = x;
      iLen = 0;
      } /* while */

   x += iLen;
   *pDest++ = x;
   *pDest++ = x; // Store a few more XSIZE to end the line
   *pDest++ = x; // so that the compressor doesn't go past
   *pDest++ = x; // the end of the line
} /* G4ENCEncodeLine() */

//
// Reverse the bit order of the data
//
static void G4ENCReverse(uint8_t *pData, int iLen)
{
    for (int i=0; i<iLen; i++) {
        pData[i] = ucMirror[pData[i]];
    }
} /* G4ENCReverse() */
//
// Compress a line of pixels and add it to the output
// the input format is expected to be MSB (most significant bit) first
// for example, pixel 0 is in byte 0 at bit 7 (0x80)
// Returns G4ENC_SUCCESS for each line if all is well and G4ENC_IMAGE_COMPLETE
// for the last line
//
int G4ENC_addLine(G4ENCIMAGE *pImage, uint8_t *pPixels)
{
int16_t a0, a0_c, b2, a1;
int dx;
int xsize, iErr;
int iCur, iRef, iLen;
int iHighWater;
int16_t *CurFlips, *RefFlips;
BUFFERED_BITS bb;

    if (pImage == NULL || pPixels == NULL)
        return G4ENC_INVALID_PARAMETER;
    if (pImage->ucFillOrder != G4ENC_MSB_FIRST && pImage->ucFillOrder != G4ENC_LSB_FIRST)
        return G4ENC_NOT_INITIALIZED;
    memcpy(&bb, &pImage->bb, sizeof(BUFFERED_BITS)); // keep local copy
    CurFlips = pImage->pCur;
    RefFlips = pImage->pRef;
    iErr = 0;
    xsize = pImage->iWidth; /* For performance reasons */

    iHighWater = OUTPUT_BUF_SIZE - 8;
    // Convert the incoming line of pixels into run-end data
    G4ENCEncodeLine(pPixels, pImage->iWidth, CurFlips);

      /* Encode this line as G4 */
      a0 = a0_c = 0;
      iCur = iRef = 0;
      while (a0 < xsize)
         {
         b2 = RefFlips[iRef+1];
         a1 = CurFlips[iCur];
         if (b2 < a1) /* Is b2 to the left of a1? */
            {
            /* yes, do pass mode */
            a0 = b2;
            iRef += 2;
            G4ENCInsertCode(&bb, 1, 4); /* Pass code = 0001 */
            }
         else /* Try vertical and horizontal mode */
            {
            dx = RefFlips[iRef] - a1;  /* b1 - a1 */
            if (dx > 3 || dx < -3) /* Horizontal mode */
               {
                   G4ENCInsertCode(&bb, 1, 3); /* Horizontal code = 001 */
               if (a0_c) /* If currently black */
                  {
                      G4ENCAddBlack(CurFlips[iCur] - a0, &bb);
                      G4ENCAddWhite(CurFlips[iCur+1] - CurFlips[iCur], &bb);
                  }
               else /* currently white */
                  {
                      G4ENCAddWhite(CurFlips[iCur] - a0, &bb);
                      G4ENCAddBlack(CurFlips[iCur+1] - CurFlips[iCur], &bb);
                  }
               a0 = CurFlips[iCur+1]; /* a0 = a2 */
               if (a0 != xsize)
                  {
                  iCur += 2; /* Skip two color flips */
                  while (RefFlips[iRef] != xsize && RefFlips[iRef] <= a0)
                     iRef += 2;
                  }
               } /* horizontal mode */
            else /* Vertical mode */
               {
               dx = (dx + 3) * 2; /* Convert to index table */
                   G4ENCInsertCode(&bb, vtable[dx], vtable[dx+1]);
               a0 = a1;
               a0_c = 1-a0_c;
               if (a0 != xsize)
                  {
                  if (iRef != 0)
                     iRef -= 2;
                  iRef++; /* Skip a color change in cur and ref */
                  iCur++;
                  while (RefFlips[iRef] <= a0 && RefFlips[iRef] != xsize)
                      iRef += 2;
                  }
               } /* vertical mode */
            } /* horiz/vert mode */
         } /* while x < xsize */
    iLen = (int)(bb.pBuf-pImage->ucFileBuf);
   if (iLen >= iHighWater) // need to dump some data
   {
       if (pImage->ucFillOrder == G4ENC_LSB_FIRST) { // need to reverse the bits
           G4ENCReverse(pImage->ucFileBuf, iLen);
       }
          // Our internal buffer is full, do we copy it to the user supplied buffer or pass it to the WRITE callback?
          if (pImage->pfnWrite) { // pass the data to the callback
              (*pImage->pfnWrite)(pImage->ucFileBuf, iLen);
          } else { // the user supplied a buffer check; if we hit the end
              if (pImage->iDataSize + iLen >= pImage->iOutSize) {// not enough space
                  pImage->iError = iErr = G4ENC_DATA_OVERFLOW; // we don't have a better error
                  return iErr;
              }
              // we're good to go
              memcpy(&pImage->pOutBuf[pImage->iDataSize], pImage->ucFileBuf, iLen);
          }
          pImage->iDataSize += iLen;
          bb.pBuf = pImage->ucFileBuf; // reset to start of output buffer
      }
    if (pImage->y == pImage->iHeight-1) { // last line of image
      /* Add two EOL's to the end for RTC */
        G4ENCInsertCode(&bb, 1, 12); /* EOL */
        G4ENCInsertCode(&bb, 1, 12); /* EOL */
        G4ENCFlushBits(&bb); // output the final buffered bits
        // wrap up final output
        iLen = (int)(bb.pBuf-pImage->ucFileBuf);
        if (pImage->ucFillOrder == G4ENC_LSB_FIRST) { // need to reverse the bits
            G4ENCReverse(pImage->ucFileBuf, iLen);
        }
          if (pImage->pfnWrite) { // pass the data to the callback
              (*pImage->pfnWrite)(pImage->ucFileBuf, iLen);
          } else { // the user supplied a buffer; check if we hit the end
              if (pImage->iDataSize + iLen >= pImage->iOutSize) {// not enough space
                  pImage->iError = iErr = G4ENC_DATA_OVERFLOW; // we don't have a better error
                  return iErr;
              }
          // we're good to go
          memcpy(&pImage->pOutBuf[pImage->iDataSize], pImage->ucFileBuf, iLen);
          } // user supplied buffer
        pImage->iDataSize += iLen;
        iErr = G4ENC_IMAGE_COMPLETE;
    }
    pImage->pCur = RefFlips; // swap current and reference lines
    pImage->pRef = CurFlips;
    pImage->y++;
    memcpy(&pImage->bb, &bb, sizeof(bb));
    return iErr;
} /* G4ENC_addLine() */
//
// Copy a line of pixels from a OneBitDisplay library image buffer
// This function is here as a convenience to use image data from my
// OneBitDisplay library since the memory is oriented differently.
//
void G4ENC_getOBDLine(int iWidth, uint8_t *pImage, int iLine, uint8_t *pPixels)
{
uint8_t *s, *d, uc, ucSrcMask, ucDstMask;
int x;
    
    ucDstMask = 0x80;
    ucSrcMask = 1 << (iLine & 7);
    uc = 0;
    d = pPixels;
    s = &pImage[(iLine >> 3) * iWidth];
    for (x=0; x<iWidth; x++) {
        if (s[x] & ucSrcMask)
            uc |= ucDstMask;
        ucDstMask >>= 1;
        if (ucDstMask == 0) { // time to write it
            *d++ = ~uc;
            uc = 0;
            ucDstMask = 0x80;
        }
    } // for x
} /* G4ENC_getOBDLine() */
//
// Returns the number of bytes of G4 created by the encoder
//
int G4ENC_getOutSize(G4ENCIMAGE *pImage)
{
    int iSize = 0;
    if (pImage != NULL)
        iSize = pImage->iDataSize;
    return iSize;
} /* getOutSize() */

int G4ENC_getTIFFHeaderSize(void)
{
    return (G4ENC_TAG_COUNT * 12) + 14 + (int)strlen(SOFTWARE)+1;
} /* getTIFFHeaderSize() */

//
// Add a TIFF tag to the header output
//
static int G4ENCAddTIFFTag(uint8_t *pOut, int iOff, int iTag, int iCount, uint8_t iType, int iValue)
{
    pOut[iOff] = (uint8_t)iTag; // uint16_t tag number
    pOut[iOff+1] = (uint8_t)(iTag >> 8);
    pOut[iOff+2] = iType; // uint16_t tag type
    pOut[iOff+3] = 0x00;
    pOut[iOff+4] = (uint8_t)iCount; // uint32_t value count
    pOut[iOff+5] = (uint8_t)(iCount >> 8);
    pOut[iOff+6] = (uint8_t)(iCount >> 16);
    pOut[iOff+7] = (uint8_t)(iCount >> 24);
    pOut[iOff+8] = (uint8_t)iValue; // uint32_t or uint16_t value
    pOut[iOff+9] = (uint8_t)(iValue >> 8);
    pOut[iOff+10] = (uint8_t)(iValue >> 16);
    pOut[iOff+11] = (uint8_t)(iValue >> 24);
    return iOff+12;
} /* G4ENCAddTIFFTag() */

int G4ENC_getTIFFHeader(G4ENCIMAGE *pImage, uint8_t *pOut)
{
    int iOff = 0; // output offset
    
    if (pImage == NULL || pOut == NULL)
        return G4ENC_INVALID_PARAMETER;
    
    if (pImage->ucFillOrder != G4ENC_MSB_FIRST && pImage->ucFillOrder != G4ENC_LSB_FIRST)
        return G4ENC_NOT_INITIALIZED;
    
    // Create a TIFF file header byte by byte, then tag by tags
    pOut[iOff++] = 'I'; // Intel (little-endian) byte order
    pOut[iOff++] = 'I';
    pOut[iOff++] = 0x2a; // TIFF Version 4.2
    pOut[iOff++] = 0x00;
    pOut[iOff++] = 0x08; // uint32_t offset to IFD
    pOut[iOff++] = 0x00;
    pOut[iOff++] = 0x00;
    pOut[iOff++] = 0x00;
    pOut[iOff++] = G4ENC_TAG_COUNT; // uint16_t tag count
    pOut[iOff++] = 0x00;
    iOff = G4ENCAddTIFFTag(pOut, iOff, 256, 1, G4ENC_TAG_SHORT, pImage->iWidth);
    iOff = G4ENCAddTIFFTag(pOut, iOff, 257, 1, G4ENC_TAG_SHORT, pImage->iHeight);
    iOff = G4ENCAddTIFFTag(pOut, iOff, 258, 1, G4ENC_TAG_SHORT, 1); // bits per sample
    iOff = G4ENCAddTIFFTag(pOut, iOff, 259, 1, G4ENC_TAG_SHORT, 4); // compression
    iOff = G4ENCAddTIFFTag(pOut, iOff, 262, 1, G4ENC_TAG_SHORT, 0); // photometric interpretation - white is zero
    iOff = G4ENCAddTIFFTag(pOut, iOff, 266, 1, G4ENC_TAG_SHORT, pImage->ucFillOrder); // bit fill order (direction)
    iOff = G4ENCAddTIFFTag(pOut, iOff, 273, 1, G4ENC_TAG_LONG, 14+(G4ENC_TAG_COUNT*12)+(int)strlen(SOFTWARE)+1); // strip offsets
    iOff = G4ENCAddTIFFTag(pOut, iOff, 277, 1, G4ENC_TAG_SHORT, 1); // samples per pixel
    iOff = G4ENCAddTIFFTag(pOut, iOff, 278, 1, G4ENC_TAG_SHORT, pImage->iHeight); // rows per strip
    iOff = G4ENCAddTIFFTag(pOut, iOff, 279, 1, G4ENC_TAG_SHORT, pImage->iDataSize); // strip byte counts
    iOff = G4ENCAddTIFFTag(pOut, iOff, 305, (int)strlen(SOFTWARE)+1, G4ENC_TAG_ASCII, iOff+16); // Software
    pOut[iOff++] = 0; // terminating IFD = 0x00000000
    pOut[iOff++] = 0;
    pOut[iOff++] = 0;
    pOut[iOff++] = 0;
    memcpy(&pOut[iOff], SOFTWARE, strlen(SOFTWARE)+1);
    return G4ENC_SUCCESS;
} /* G4ENC_getTIFFHeader() */
