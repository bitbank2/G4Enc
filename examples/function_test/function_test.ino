//
// G4 Encoder Function Test
//
#include <G4ENCODER.h>
#include "bart_73x200_bmp.h"
#include "bart_tif.h"

G4ENCODER g4;
#define WIDTH 256
#define HEIGHT 128
static uint8_t ucPixels[WIDTH];
static uint8_t ucTemp[2048]; // output buffer to hold compressed data
//
// Simple logging print
//
void TIFFLOG(int line, char *string, const char *result)
{
    Serial.printf("Line: %d: msg: %s%s\n", line, string, result);
} /* TIFFLOG() */

void setup() {
    //int i, iTime1, iTime2;
    int iSize, y, rc, iPitch;
    uint8_t *s;
//    uint8_t *pFuzzData;
    char *szTestName;
    const char *szStart = " - START";

    Serial.begin(115200);
    delay(3000); // wait for CDC-Serial to start

    // Test 1 - the encoder respects the output buffer size
    szTestName = (char *)"G4 encode, insuffient output buffer";
    TIFFLOG(__LINE__, szTestName, szStart);
    rc = g4.init(WIDTH, HEIGHT, G4ENC_MSB_FIRST, NULL, ucTemp, sizeof(ucTemp)); // write to existing buffer
    if (rc == G4ENC_SUCCESS) {
        for (y=0; y<HEIGHT && rc == G4ENC_SUCCESS; y++) {
          // create a worst-case image with a 1-pixel checkerboard pattern
          if (y & 1) {
              memset(ucPixels, 0x55, sizeof(ucPixels));
          } else {
              memset(ucPixels, 0xaa, sizeof(ucPixels));
          }
        rc = g4.addLine(ucPixels);
        } // for y
        if (rc == G4ENC_DATA_OVERFLOW) {
            TIFFLOG(__LINE__, szTestName, " - PASSED");
        } else {
            TIFFLOG(__LINE__, szTestName, " - FAILED");
            Serial.printf("Worst case output didn't overflow - something went wrong\n");
        }
    } else {
        TIFFLOG(__LINE__, szTestName, " - FAILED");
        Serial.printf("g4.init() returned %d\n", rc);
    }
    
    // Test 2 - no stray bytes are written past the end of the output buffer
    szTestName = (char *)"G4 encode, end of buffer not overwritten";
    TIFFLOG(__LINE__, szTestName, szStart);
    *(uint32_t *)&ucTemp[sizeof(ucTemp)/2] = 0x12345678; // test pattern
    rc = g4.init(WIDTH, HEIGHT, G4ENC_MSB_FIRST, NULL, ucTemp, sizeof(ucTemp)/2); // write to existing buffer
    if (rc == G4ENC_SUCCESS) {
        for (y=0; y<HEIGHT && rc == G4ENC_SUCCESS; y++) {
          // create a worst-case image with a 1-pixel checkerboard pattern
          if (y & 1) {
              memset(ucPixels, 0x55, sizeof(ucPixels));
          } else {
              memset(ucPixels, 0xaa, sizeof(ucPixels));
          }
        rc = g4.addLine(ucPixels);
        } // for y
        if (rc == G4ENC_DATA_OVERFLOW && *(uint32_t *)&ucTemp[sizeof(ucTemp)/2] == 0x12345678) {
            TIFFLOG(__LINE__, szTestName, " - PASSED");
        } else {
            TIFFLOG(__LINE__, szTestName, " - FAILED");
            Serial.printf("Worst case output didn't overflow - something went wrong\n");
        }
    } else {
        TIFFLOG(__LINE__, szTestName, " - FAILED");
        Serial.printf("g4.init() returned %d\n", rc);
    }
    // Test 3 - compress a known image and check for correct output
    szTestName = (char *)"G4 encode, check correctness of output";
    TIFFLOG(__LINE__, szTestName, szStart);
    rc = g4.init(73, 200, G4ENC_MSB_FIRST, NULL, ucTemp, sizeof(ucTemp)); // write to existing buffer
    s = (uint8_t *)&bart_73x200_bmp[0x92]; // start of bitmap data (upside down)
    iPitch = (73 + 7) >> 3;
    iPitch = (iPitch + 3) & 0xfffc; // DWORD aligned for Windows BMP files
    s += 199 * iPitch; // bottom up bitmap
    if (rc == G4ENC_SUCCESS) {
        for (y=0; y<200 && rc == G4ENC_SUCCESS; y++) {
            rc = g4.addLine(s);
            s -= iPitch;
        } // for y
        iSize = g4.getOutSize();
        if (rc == G4ENC_IMAGE_COMPLETE && iSize == sizeof(bart_tif) && memcmp(ucTemp, bart_tif, iSize) == 0) {
            TIFFLOG(__LINE__, szTestName, " - PASSED");
        } else {
            TIFFLOG(__LINE__, szTestName, " - FAILED");
            if (iSize != sizeof(bart_tif))
                Serial.printf("Output size doesn't match\n");
            else
                Serial.printf("Output data doesn't match");
        }
    } else {
        TIFFLOG(__LINE__, szTestName, " - FAILED");
        Serial.printf("g4.init() returned %d\n", rc);
    }

    // Test 4 - test TIFF header creation
    szTestName = (char *)"G4 encode, check TIFF header creation";
    TIFFLOG(__LINE__, szTestName, szStart);
    rc = g4.init(73, 200, G4ENC_MSB_FIRST, NULL, ucTemp, sizeof(ucTemp)); // write to existing buffer
    s = (uint8_t *)&bart_73x200_bmp[0x92]; // start of bitmap data (upside down)
    iPitch = (73 + 7) >> 3;
    iPitch = (iPitch + 3) & 0xfffc; // DWORD aligned for Windows BMP files
    s += 199 * iPitch; // bottom up bitmap
    if (rc == G4ENC_SUCCESS) {
        for (y=0; y<200 && rc == G4ENC_SUCCESS; y++) {
            rc = g4.addLine(s);
            s -= iPitch;
        } // for y
        iSize = g4.getTIFFHeaderSize();
        g4.getTIFFHeader(ucPixels); // copy the header here
        y = 1; // assume header is valid
        if (ucPixels[0] != 'I' || ucPixels[1] != 'I') { // Intel byte order
            y = 0;
        }
        if (ucPixels[8] != 11) { // number of TIFF tags (a fixed value)
            y = 0;
        }
        if (memcmp(&ucPixels[0x92], "Created with G4ENCODER by Larry Bank", 36) != 0) { // check creator message
            y = 0;
            Serial.printf("TIFF Creator tag has been modified.\n");
        }
        if (rc == G4ENC_IMAGE_COMPLETE && iSize == 183 && y == 1) {
            TIFFLOG(__LINE__, szTestName, " - PASSED");
        } else {
            TIFFLOG(__LINE__, szTestName, " - FAILED");
            if (iSize != 4+(11*12))
                Serial.printf("TIFF header size doesn't match\n");
            else
                Serial.printf("Invalid TIFF header data");
        }
    } else {
        TIFFLOG(__LINE__, szTestName, " - FAILED");
        Serial.printf("g4.init() returned %d\n", rc);
    }
} /* setup() */

void loop()
{

} /* loop() */