# Mir
This is an intel hex parser and CRC appender.  The program will go through an intel hex file and calculate a crc for it.
Readme is currently for tagged version 2.3.5.37!

## What is Mir?
Mir is a Checksum appending tool for embedded development.  To ensure prevention of malicious code execution on an embedded device, a run-time integrity check from the bootloader
is performed on flash memory of the microcontroller and it verifies this calculation with a calculation done after the binary was built on a PC.  But how does that original checksum
get into the micro's flash?  That's what this tool does.  It parses an intel hex file, extracts the data from each record, converts the ASCII to hexadecimal, then calculates a
checksum for it and appends that, along with the size of the binary, to a record of your choice based on your memory map!

## How can it help me as an embedded developer?
Mir can help by preventing the exercise in futility of writing an intel hex parser from scratch for this exact purpose.  Checksums are either done in software and hardware on a micro
and this tool allows you to implement your own VERY quickly!  Instead of spending weeks designing and testing your own tool, just use Mir and apply your own checksum/CRC routine to
the program and config file and you have your information ready to flash!

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See deployment for notes on how to deploy the project on a live system.

### Prerequisites

This current version does not have the configurator ready yet.  So you will need GCC, make, and CMake.
You can use Visual Studio in Windows and CMake directly on Linux, or Mac.

```
GCC, make, and CMake
```

### OS Compatibility

If you can build it on the OS, it should work!

### Installing

A step by step series of examples that tell you how to get a development env running

Download the repo from either master, or tagged release.

```
Navigate to the current directory
```

```
type 'make'
```

```
Output should be created as an executable "mir.exe"
Issue "./mir" to invoke the program
```
Append this execution to the end of a build sequence and have the program in appender mode to have it in your development chain.


To verify it installed correctly, go into mirConfig.mir and change the address field to the correct appendage address desired. and run the executable.

Output should read 
```
File Size: 0xAAAA  File CRC: 0xAAAA
Total execution time: x.xxxxxx ms
File Copied Successfully
```

In order to install the GUI configurator, use CMake and set the source code directory to:
```
/Mir/intelHexParser/mirConfigurator
```

and the build binaries to:
```
/Mir/intelHexParser/mirConfigurator/build
```

Click on Configure and wait for this to complete.  Once it has completed, click Generate.
Once this has finished, you may open the project to build.  Build it and you are good to go!

## Setup
Mir offers a few modes based on config values:
```
1. Appender Mode - Runs and executes the checksum calculation on the file and paths inside config file.  Applied using a post-build script.
2. UI Mode - For the purpose of seeing what the program is doing with your hex file.  With the debug flag on, it prints every byte it analyzes.
3. Test mode - A custom shell for testing different parts of the program that you can implement!
```

Let's say that you have a memory map designed and the address you want it appended the checksum and length to is: 0x0804FFF0.
You will have to edit the config file in order to make this possible.  /scripts has the mir config file present.

## Configuration File

Inside the configuration file, there are 4 possible records in the current release, there will be more added and it is expected there should be more added as well.

Example of config file
```
UI:00;
CRC:02;
Byte:00;
Append:0804FFF0;
FileLoc:;
FileName:app.hex;
OpName:appCRC.hex;
OpLoc:;
```

1st record - The Enum number for the operating mode, currently, here they are from config.h
```
00    MIR_APPENDER_MODE
01    MIR_UI_MODE
02    MIR_TEST_MODE
03    MIR_NUM_MODES
```
2nd record - The Enum number for the CRC function-pointer array index, currently, here they are from config.h  These should be added if a new CRC function is desired and function pointer should be appended to the correct array.
```
00    CRC16CCIT,
01    CRC16_X_MOD,
02    CRC16_IEC_60335,
```
3rd record - The Enum number for the byte order, currently, here they are from config.h
```
00    MIR_LITTLE_ENDIAN,
01    MIR_BIG_ENDIAN,
02    MIR_MIDDLE_ENDIAN,
03    MIR_PDP_ENDIAN,
04    MIR_HONEYWELL316_ENDIAN,
05    MIR_BYTE_SWAPPED,
```
4th record - The address to append the calculated CRC to.  Should be defined by the user based on memory map.
Example
```
0803FF87
-> read by program as 0x0803FF87
```

With all directory information, no input means that this record will append it to the current directory.

Example
```
FileLoc:;
```
5th record - The file directory to run the tool on.

6th record - The filename to calculate the CRC on.

7th record - The name of the output and CRC & Length appended file modified from the original file.

8th record - The output file location directory

9th record - This is to account for a "version" field placed in the record which is up to 4-bytes long BEFORE the length/CRC area.

Example 
```
00    VERSION_FIELD_DOES_NOT_EXIST,
01    VERSION_FIELD_EXISTS,
```

## Adding My Own Checksum Function
To add your own function follow these steps:
```
1. Open config.h, add to the "CRC_FUNCTIONS_t" (On line 49) enum another entry and make note of the enum value.
2. Open the configuration file and change the second record "UI" to "UI:0x" where x is the enum value of your new checksum function.
3. Open utils.c and add the checksum function logic into the file.
4. Open utils.h and add the function prototype of the checksum function you implemented in utils.c.
5. Open config.c and go to line 33.  Add that function pointer into this array in the specified index of the enum you just added in config.h.
```
This implements a custom checksum calculation and appends it to the address you need in the hex file!

## Adding to my post build process
Depending on which IDE or build system you use, make sure you have two things enabled first.
```
1. Intel Hex File is generated
2. Mir is invoked in your post-build script
```

Once these two steps are completed do the following:
```
1. Make sure the program is built with the DEBUG_FLAG in main.h turned to false.
2. Find where your program generates your memory-mapped intel hex file after build and set this path in the config file record "FileLoc"
3. Set the "FileName" record in the config file to whatever your intel hex file is named in the build location.
4. Make sure the "UI" record in the config file is set to APPENDER_MODE or "00".
5. Verify that your checksum and length location inside the micro's flash is specified in the "Append" record of the config file in hexadecimal, minus the "0x".
6. Choose your crc appended file name and add this to the "OpName" record in the config file.
7. Change the "OpLoc" record in the config file to the location you want the completed hex file to be placed.
8. Make sure the "Byte" record in the config file matches what your architecture on the micro is.
9. Verify the function pointer index you want for your custom checksum function is the same as it is in the "CRC" record of the config file. "01" would be 0x01.
10. Build and verify the program runs and appends the length and checksum correctly!
```

## Debug Information
To assist with debugging information, there is a DEBUG_ACTIVE precomplied flag inside of ```main.h```, please switch it to true if you desire to see all debugging information as "printf".

##

## Built With

* [GCC Make](https://gcc.gnu.org/onlinedocs/gccint/Makefile.html) - Build tool
* [CMake](https://cmake.org/) - Build tool

## Contributing

Currently a WIP, but make pull requests and I will approve ones I see as okay.

## Versioning

For the versions available, see the [tags on this repository](https://github.com/ZygalM1S1U/Mir_Appender/releases).

## Authors

* **James Novorita** - *All Development* - [ZygalM1S1U](https://github.com/ZygalM1S1U), [Professional Website](https://zygalsplayground.com/)

## License

This project is licensed under the MIT License - see the [LICENSE.md](https://github.com/ZygalM1S1U/Mir_Appender/blob/master/LICENSE) file for details

## Acknowledgments

* Special thanks to Bryan for help with CMake and ImGui
