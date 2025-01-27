//
// This example shows how to take a screenshot
// of the current eink display (M5Stack PaperS3 for now)
// and save it to the SD card as a TIFF G4 file
//
// written by Larry Bank (bitbank@pobox.com)
// Project started 1/27/2025
//
#include <FastEPD.h>
#include <G4ENCODER.h>
#include <SPI.h>
#include <SD.h>

// M5Stack PaperS3 uSD card GPIO numbers
#define SD_CS 47
#define SD_SCK 39
#define SD_MOSI 38
#define SD_MISO 40

FASTEPD epd;
G4ENCODER g4;

void SaveScreenshot()
{
  int y, rc, iSize, iOutSize, iBufferSize, iPitch;
  uint8_t *pOut, *pImage, *pHeader;
  File myfile;

  pImage = epd.currentBuffer();
  iPitch = epd.width()/8; // source pitch
  iBufferSize = iPitch * (epd.height()/3); // assumes we'll compress at least 3:1
  pOut = (uint8_t *)malloc(iBufferSize); // big enough
  if (!pOut) {
    Serial.printf("Error allocating %d bytes, aborting...\n", iBufferSize);
    return;
  }
  rc = g4.init(epd.width(), epd.height(), G4ENC_MSB_FIRST, NULL, pOut, iBufferSize);
  if (rc == G4ENC_SUCCESS) {
    for (y=0; y<epd.height() && rc == G4ENC_SUCCESS; y++) {
      rc = g4.addLine(&pImage[iPitch*y]);
    } // for y
    if (rc == G4ENC_IMAGE_COMPLETE) {
      iSize = g4.getOutSize();
      Serial.printf("%dx%d compressed to %d bytes of G4 data\n", epd.width(), epd.height(), iSize);
      // Create a file and store the data as a TIFF
      myfile = SD.open("/screenshot.tif", FILE_WRITE);
      if (!myfile) {
        Serial.println("Error creating output file");
        free(pOut);
        return;
      } else {
        Serial.println("file created");
      }
      pHeader = &pOut[iSize]; // share a little part of the same buffer
      g4.getTIFFHeader(pHeader);
      Serial.printf("Header size = %d bytes\n", g4.getTIFFHeaderSize());
      myfile.write(pHeader, g4.getTIFFHeaderSize()); // write the TIFF header first
      myfile.write(pOut, iSize); // write the G4 data
      myfile.flush();
      myfile.close(); // we're done
      Serial.println("TIFF file creation complete");
      free(pOut);
    }
  } else {
    Serial.printf("G4 init failed with error %d\n", rc);
  }
} /* SaveScreenshot() */

void DrawScene()
{
  int i, x, y, cx, cy;
  int iWidth = epd.width();
  int iHeight = epd.height();
    // Draw some random circles, lines and rectangles
  for (i=0; i<200; i++) { // 2oo objects is enough
    x = rand() % iWidth;
    y = rand() % iHeight;
    switch (rand() & 3) {
      case 0: // line
        cx = rand() % iWidth;
        cy = rand() % iHeight;
        epd.drawLine(x, y, cx, cy, BBEP_BLACK);
        break;
      case 1: // rectangle
        cx = rand() % 100;
        cy = rand() % 100;
        epd.fillRect(x, y, cx, cy, BBEP_BLACK);
        break;
      case 2: // circle
        cx = rand() % 50;
        epd.fillCircle(x, y, cx, BBEP_BLACK);
        break;
    } // switch
  } // for i
  epd.setFont(FONT_12x16);
  epd.setTextColor(BBEP_BLACK, BBEP_WHITE);
  epd.setCursor(0,0);
  epd.println("FastEPD Screenshot Example Sketch");
  epd.println("Saving this frame as a TIFF G4 file...");
} /* DrawScene() */

void setup()
{
  int rc;
  Serial.begin(115200);
  delay(3000); // allow time for CDC-serial to begin

  SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
  while (!SD.begin(SD_CS, SPI, 10000000)) {
      Serial.println("Unable to access SD card");
      delay(1000);
  }
  Serial.println("SD card success");
  rc = epd.initPanel(BB_PANEL_M5PAPERS3);
  Serial.printf("initPanel returned %d\n", rc);
  epd.fillScreen(BBEP_WHITE); // defaults to 1-bpp mode
  DrawScene();
  epd.fullUpdate(true, false);
  SaveScreenshot();
} /* setup() */

void loop()
{
}

