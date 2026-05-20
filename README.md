# Micropolis Deluxe

This is a C++/SDL2 port of the SimCity/Micropolis code developed by Don Hopkins for Unix. See __README_OG for the original readme provided by Don Hopkins and EA.

Thanks to Leeor Dicker for starting this SDL2 port. I forked his code in July 2025, it was a great foundation for me to elaborate and complement over for fun!

## License
This project is licensed under the GPL as required by the original code. See COPYING for details. There are additional licensing stipulations required by EA, see "ADDITIONAL TERMS" in __README_OG.

## Goals
* Port the original code to C++
* Port all drawing/input/window management code to SDL2
* Eliminate dependency on TCL/Tk
* Publish on the Microsoft Store and the Mac App Store

## Platforms
My development machines are Windows and MacOS based so the current development efforts will target Windows 10 and 11 (x64) and latest MacOS (Intel, Apple Silicon). With some minor changes (notably in the filesystem handling code) this code base should work on other major platforms.
