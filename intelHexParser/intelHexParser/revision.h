#ifndef _REVISION_H__
#define _REVISION_H__

#include "main.h"

#define MIR_MAJOR   2
#define MIR_MINOR   3
#define MIR_BUILD   5
#define MIR_DEV     35

#endif // _REVISION_H__

/* Build notes:
   Mir version 2.3.5.35
   Second major release
   Application features:
   - The application has three modes of operation: Appender mode, Test Mode, and UI mode
        Appender Mode: Based on the file path, it will parse an intel hex file, calculate a CRC, and append both the length and crc to the file.
        Test Mode: Allows testing of newly added CRC functions (Placeholde rin for development)
        UI Mode: Will walk you through each piece of the program and allow you to see the inner workings
   - The configuration settings are starting to be put in place.  ImGui will be added for mking config files.
   - CRC functions can be added by following the function pointer protocol specified in config.c  Make sure to update the ENUM and the function pointers are updated correctly.
   - Basic funtionality is implemented.  Correctly calculates a CRC for a hex file.
   - Appender Mode is now active.

*/
