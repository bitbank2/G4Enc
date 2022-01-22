//
// G4ENCODER
// A CCITT G4 image encoder library
// written by Larry Bank
// bitbank@pobox.com
// Arduino port started 1/20/2022
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
	return (G4ENC_TAG_COUNT * 12) + 14 + strlen(SOFTWARE);
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
