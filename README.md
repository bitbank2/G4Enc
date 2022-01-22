G4ENCODER<br>
-----------------------------------
Copyright (c) 2022 BitBank Software, Inc.<br>
Written by Larry Bank<br>
larry@bitbanksoftware.com<br>
<br>
**I optimize other people's code for a living. This library is a good example of the kind of work I do for my commercial clients; it contains many unique and clever optimizations that allows it to perform better than anything else available. I'm happy to contribute optimized libraries to the open source community in addition to working on commercial projects. Whatever platform you're using, I can make significant improvements to your native code. Please contact me so that I can show you how.**<br>
<br>
What is it?
------------
An 'embedded-friendly' (aka Arduino) CCITT Group 4 (ITU T.6) image encoding library<br>
<br>
Why did you write it?
---------------------
Starting in the late 80's I wrote my own imaging codecs for the existing standards and was fascinated with FAX (CCITT G3/G4) image compression. I optimized the code over the intervening years and have used it to great effect in several products. recently I've been experimenting with e-ink panels and wireless electronic shelf labels. G4 compression seemed like a good fit for the type of image data you would want to work with. G4 compression at its heart is run-length encoded (repeating runs of black and white pixels). The 2D aspect allows it to compress some images extremely well. It seemed like a great fit for running on low power microcontrollers since the algorithm is relatively simple, doesn't need much memory and uses all integer math. Hopefully by providing an encoder here along with my TIFF_G4 decoder library, it will find its way into more modern projects.<br>

Features:<br>
---------<br>
- Supports any MCU with at least 16K of RAM (Cortex-M0+ is the simplest I've tested)
- Optimized for speed; the main limitation will be how fast you can copy the pixels to the display.
- TIFF G4 image data can come from memory (FLASH/RAM), SDCard or any media you provide.
- CCITT G4 data can be raw (you provide size info), or contained in a TIFF file structure.
- Simple class and callback design allows you to easily add TIFF G4 support to any application.
- The C code doing the heavy lifting is completely portable and has no external dependencies.
- Includes fast anti-aliasing options (2 or 4-bits per pixel output).
- Can scale the decoded image by any fractional amount (smaller or larger).
- The only code required is a callback function to use the pixels (emitted one line at a time).

Acquiring TIFF files:
---------------------
You'll notice that the images provided in the test_images folder have been turned into C code. Each byte is now in the form 0xAB so that it can be compiled into your program and stored in FLASH memory alongside your other code. You can use a command line tool called xxd to convert a binary file into this type of text. If you use xxd, make sure to add a const/PROGMEM modifier in front of the TIFF data array to ensure that it gets written to FLASH and not RAM by your build environment. I've also written my own tool to do this (https://github.com/bitbank2/bin_to_c). My tool already adds the necessary prefixes for Arduino FLASH data.<br>

The Callback functions:
-----------------------
One of the ways to allow this code to run on any embedded platform was to define a set of callback functions. These isolate the TIFF decoding logic from the display and file I/O. This allows the core code to run on any system, but you need to help it a little. At a minimum, your code must provide a function to draw (or store) each line of image pixels emitted by the library. If you're displaying a TIFF file from memory (RAM or FLASH), this is the only function you need to provide. In the examples folder there are multiple sketches to show how this is done on various display libraries. For reading from SD cards, 4 other functions must be provided: open, close, read, seek. There is an example for implementing these in the examples folder as well.
Note:
If you're using the ESP32 or ESP8266 (or another MCU which uses the Harvard Architecture) and decoding TIFF images stored in RAM or FLASH, you'll need to use the correct open function (openRAM or openFLASH). For MCUs based on the ARM Cortex-M, they are interchangeable.

The API:
--------
Please consult the [Wiki](https://github.com/bitbank2/TIFF_G4/wiki) for detailed info about each method exposed by the TIFFG4 class. I've also provided a C interface to the library and example code which compiles from a makefile for Linux.<br>


If you find this code useful, please consider becoming a sponsor or sending a donation.

[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=SR4F44J2UR8S4)

