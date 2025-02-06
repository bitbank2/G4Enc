//
// This example shows how to use 1-bit per pixel text and drawing
// functions from my bb_epaper library and compress the
// output into a highly compressed (losslessly) TIFF G4 file
// Then write the file to a micro-SD card
//
#include <G4ENCODER.h>
#include <bb_epaper.h>
#include <SPI.h>
#include <SD.h>
#include "Roboto_Black_38.h"

BBEPAPER epd(EP_PANEL_UNDEFINED);
G4ENCODER g4;
#define WIDTH 640
#define HEIGHT 480
// Assume uncompressed as the worst case output size
#define MAX_OUTPUT_SIZE (WIDTH * HEIGHT / 8)
uint8_t *pBuffer, *pOut;
// M5Stack PaperS3 uSD card GPIO numbers
#define SD_CS 47
#define SD_SCK 39
#define SD_MOSI 38
#define SD_MISO 40
//
// Compress the frame as CCITT G4 data
//
int CompressAsG4(void)
{
  int rc, y, iPitch, iSize = 0;

  iPitch = (WIDTH+7)/8; // bytes per line
  rc = g4.init(WIDTH, HEIGHT, G4ENC_MSB_FIRST, NULL, pOut, MAX_OUTPUT_SIZE); // write to existing buffer
  if (rc == G4ENC_SUCCESS) {
    for (y=0; y<HEIGHT && rc == G4ENC_SUCCESS; y++) {
      // The pixel for
      rc = g4.addLine(&pBuffer[y * iPitch]);
    } // for y
    if (rc == G4ENC_IMAGE_COMPLETE)
      iSize = g4.getOutSize();
    else
      Serial.printf("Error compressing image = %d\n", rc);
  }
  return iSize;
} /* CompressAsG4() */

// Draw the graphics we want to write into a TIFF G4 file
void DrawScene(void)
{
  epd.fillScreen(BBEP_WHITE); // fill with white (1's)
  epd.drawRoundRect(8, 8, WIDTH-16, HEIGHT-16, 16, BBEP_BLACK); // draw a round rect around the display
  epd.setFont(Roboto_Black_38); // a pleasant looking font
  epd.setCursor(20, 100);
  epd.setTextColor(BBEP_BLACK, BBEP_WHITE);
  epd.print("Use bb_epaper");
  epd.setCursor(20, 174);
  epd.print("as an easy");
  epd.setCursor(20, 248);
  epd.print("way to make");
  epd.setCursor(20, 322);
  epd.print("1-bit graphics");
  epd.fillEllipse(WIDTH/2, 400, 200, 50, BBEP_BLACK);
  epd.fillEllipse(WIDTH/2, 400, 100, 25, BBEP_WHITE);
} /* DrawScene() */

void setup()
{
  Serial.begin(115200);
  delay(3000); // allow time for CDC-Serial to start
  Serial.println("TIFF G4 from drawing example");
  SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
  while (!SD.begin(SD_CS, SPI, 10000000)) {
      Serial.println("Unable to access SD card");
      delay(1000);
  }
  Serial.println("SD card success");

} /* setup() */

void loop()
{
  uint8_t *pHeader;
  File myfile;
  int iSize;

  epd.createVirtual(WIDTH, HEIGHT, 0); // Create a 1-bit virtual display (no memory yet)
  Serial.println("Created virtual");
  epd.allocBuffer(); // need to explicitly allocate a framebuffer
  pBuffer = (uint8_t *)epd.getBuffer(); // we will be accessing it directly, so get the framebuffer pointer
  Serial.printf("Allocated buffer, ptr = 0x%08x\n", (intptr_t)pBuffer);
  pOut = (uint8_t *)malloc(MAX_OUTPUT_SIZE);
  DrawScene(); // use bb_epaper drawing functions to create 1-bit graphics
  iSize = CompressAsG4();
  if (iSize > 0) {
    Serial.printf("%dx%d compressed to %d bytes of G4 data\n", epd.width(), epd.height(), iSize);
    // Create a file and store the data as a TIFF
    myfile = SD.open("/bb_epaper.tif", FILE_WRITE);
    if (!myfile) {
      Serial.println("Error creating output file");
      free(pOut);
      free(pBuffer);
      return;
    } else {
      Serial.println("file created");
    }
    pHeader = &pOut[iSize]; // share a little part of the same buffer (unused) to hold the TIFF header
    g4.getTIFFHeader(pHeader);
    Serial.printf("Header size = %d bytes\n", g4.getTIFFHeaderSize());
    myfile.write(pHeader, g4.getTIFFHeaderSize()); // write the TIFF header first
    myfile.write(pOut, iSize); // write the G4 data
    myfile.flush();
    myfile.close(); // we're done
    Serial.println("TIFF file creation complete");
  } else {
    Serial.println("Unable to write TIFF file");
  }
  free(pOut);
  free(pBuffer); // free virtual display buffer
  while (1) {};
} /* loop() */

