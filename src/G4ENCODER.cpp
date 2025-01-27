//
// G4ENCODER
// A CCITT G4 image encoder library
// written by Larry Bank
// bitbank@pobox.com
// Arduino port started 1/20/2022
// Original code written more than 20 years ago :)
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
#include "G4ENCODER.h"

// forward references
int G4ENC_init(G4ENCIMAGE *pImage, int iWidth, int iHeight, int iBitDirection, G4ENC_WRITE_CALLBACK *pfnWrite, uint8_t *pOut, int iOutSize);
int G4ENC_getTIFFHeaderSize(G4ENCIMAGE *pImage);
int G4ENC_getTIFFHeader(G4ENCIMAGE *pImage, uint8_t *pOut);
int G4ENC_addLine(G4ENCIMAGE *pImage, uint8_t *pPixels);
int G4ENC_getOutSize(G4ENCIMAGE *pImage);
void G4ENC_getOBDLine(int iWidth, uint8_t *pImage, int iLine, uint8_t *pPixels);
#include "g4enc.inl"

int G4ENCODER::init(int iWidth, int iHeight, int iBitDirection, G4ENC_WRITE_CALLBACK *pfnWrite, uint8_t *pOut, int iOutSize)
{
	return G4ENC_init(&_g4, iWidth, iHeight, iBitDirection, pfnWrite, pOut, iOutSize);
} /* init() */

int G4ENCODER::getTIFFHeaderSize()
{
	return G4ENC_getTIFFHeaderSize();
} /* getTIFFHeaderSize() */

int G4ENCODER::getTIFFHeader(uint8_t *pOut)
{
	return G4ENC_getTIFFHeader(&_g4, pOut);
} /* getTIFFHeader() */

int G4ENCODER::addLine(uint8_t *pPixels)
{
	return G4ENC_addLine(&_g4, pPixels);
} /* addLine() */

int G4ENCODER::getOutSize()
{
	return _g4.iDataSize;
} /* getOutSize() */

void G4ENCODER::getOBDLine(int iWidth, uint8_t *pImage, int iLine, uint8_t *pPixels)
{
    return G4ENC_getOBDLine(iWidth, pImage, iLine, pPixels);
} /* getOBDLine() */
