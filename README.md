# Mir
This is an intel hex parser and CRC appender.  The program will go through an intel hex file and calculate a crc for it.
Readme is currently for tagged version 2.3.5.35!

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

## Configuration File

Inside the configuration file, there are 4 possible records in the current release, there will be more added and it is expected there should be more added as well.

Example of config file
```
UI:00;
CRC:02;
Byte:00;
Append:0807FFF8;
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
01    MIR_LITTLE_ENDIAN,
02    MIR_BIG_ENDIAN,
03    MIR_MIDDLE_ENDIAN,
04    MIR_PDP_ENDIAN,
05    MIR_HONEYWELL316_ENDIAN,
06    MIR_BYTE_SWAPPED,
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

## Debug Information
To assist with debugging information, there is a DEBUG_ACTIVE precomplied flag inside of ```main.h```, please switch it to true if you desire to see all debugging information as "printf".

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
