//
// Simple example for the G4ENCODER library
// an efficient encoder for 1-bit per pixel images
// with a high compression ratio and fast encoding
//
// written by Larry Bank
// bitbank@pobox.com
// 
#include <OneBitDisplay.h>
#include <G4ENCODER.h>

OBDISP obd;
G4ENCODER g4;
#define WIDTH 256
#define HEIGHT 128
static uint8_t ucBuffer[WIDTH * ((HEIGHT+7)>>3)]; // 256x128 virtual display back buffer
static uint8_t ucTemp[2048]; // output buffer to hold compressed data

int CompressAsG4(void)
{
  int rc, y;
  int iSize = 0;
  uint8_t ucPixels[(WIDTH+7)>>3];
  
  rc = g4.init(WIDTH, HEIGHT, G4ENC_MSB_FIRST, NULL, ucTemp, sizeof(ucTemp)); // write to existing buffer
  if (rc == G4ENC_SUCCESS) {
    for (y=0; y<HEIGHT && rc == G4ENC_SUCCESS; y++) {
      // convert the memory format of OneBitDisplay into MSB-first horizontal bytes
      g4.getOBDLine(WIDTH, ucBuffer, y, ucPixels);
      rc = g4.addLine(ucPixels);
    } // for y
    if (rc == G4ENC_IMAGE_COMPLETE)
      iSize = g4.getOutSize();
  }
  return iSize;
} /* CompressAsG4() */

void setup() {
  int iSize;
  long lTime;
  
  Serial.begin(115200);
  iSize = 0;
  while (!Serial && iSize < 5) {
    delay(250); // allow USB serial time to init
    iSize++;
  }
  Serial.println("CCITT G4 (ITU T.6) lossless image encoder performance test");
  obdCreateVirtualDisplay(&obd, 256, 128, ucBuffer);
  obdFill(&obd, 0, 0);
  obdWriteString(&obd,0,0,0,(char *)"CCITT G4 Test", FONT_16x32, 0, 0);
  obdWriteString(&obd,0,0,4,(char *)"A new capability for MCUs!", FONT_8x8, 0, 0);
  lTime = millis();
  iSize = CompressAsG4();
  lTime = millis() - lTime;
  Serial.printf("%d x %d image compressed to %d bytes of G4 data in %d ms\n", WIDTH, HEIGHT, iSize, (int)lTime);
  Serial.printf("Compression ratio = %d:1\n", (256*128)/(8*iSize));
}

void loop() {
 // nothing going on here!

}
