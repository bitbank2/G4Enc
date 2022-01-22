G4ENCODER
---------
Copyright (c) 2022 BitBank Software, Inc.<br>
Written by Larry Bank<br>
larry@bitbanksoftware.com<br>

What is it?
------------
An 'embedded-friendly' (aka Arduino) CCITT Group 4 (ITU T.6) image encoding library<br>
<br>

Why did you write it?
---------------------
Starting in the late 80's I wrote my own imaging codecs for the existing standards and was fascinated with FAX (CCITT G3/G4) image compression. I optimized the code over the intervening years and have used it to great effect in several products. Recently I've been experimenting with e-ink panels and wireless electronic shelf labels. G4 compression seemed like a good fit for the type of image data you would want to transmit. G4 compression at its heart is run-length encoded (repeating runs of black and white pixels). The 2D aspect allows it to compress some images extremely well. It seemed like a great fit for running on low power microcontrollers since the algorithm is relatively simple, doesn't need much memory and uses all integer math. Hopefully by providing an encoder here along with my TIFF_G4 decoder library, it will find its way into more modern projects.<br>

Features:
---------
- Supports any MCU with at least 5K of free RAM
- Simple API allows you to easily compress 1-bpp bitmaps and optionally write a TIFF file
- Simple class and callback design allows you to easily add G4 compression to any project.
- The C code doing the heavy lifting is completely portable and has no external dependencies.

A note about G4 Compression:
----------------------------

The G4 compression algorithm is lossless - the output image is exactly the same as the input image. It was designed to compress scanned documents - black lettering on a white background. The statistical model is based on that type of image, but it also does well with solid color graphics. It can compress images very effectively if the color changes (white->black or black->white) occur within +/-3 pixels of a color change on the line above. However, this breaks down for a one pixel checkerboard pattern and the compressed data will be larger than the original. So...dithered images will perform poorly with G4. If your image is similar to the one below, it will compress quite well compared to other lossless compression algorithms.

![G4ENC](/g4_example.jpg?raw=true "G4 Example")

The API:
--------
Please consult the [Wiki](https://github.com/bitbank2/G4ENC/wiki) for detailed info about each method exposed by the G4ENCODER class. I've also provided a C interface to the library and example code which compiles from a makefile for Linux.<br>


If you find this code useful, please consider becoming a sponsor or sending a donation.

[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=SR4F44J2UR8S4)

