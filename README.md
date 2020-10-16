# Mir
This is an intel hex parser and CRC appender.  The program will go through an intel hex file and calculate a crc for it.
Readme is currently for tagged version 1.3.5.30!

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See deployment for notes on how to deploy the project on a live system.

### Prerequisites

This current version does not have the configurator ready yet.  So you will need GCC and make.

```
GCC & make
```

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
output should be created as an executable "a.exe"
```

To verify it installed correctly, go into mirConfig.mir and change the address field to the correct appendage address desired. and run the executable.

Output should read 
```
Appendage address found
End of file
File copied successfully.
```
## Configuration File

Inside the configuration file, there are 4 possible records in the current release, there will be more added and it is expected there should be more added as well.

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

Example of config file
```
UI:01;
CRC:02;
Byte:00;
Append:0803FF87;
```

## Debug Information
To assist with debugging information, there is a DEBUG_ACTIVE precomplied flag inside of ```main.h```, please switch it to true if you desire to see all debugging information as "printf".

## Built With

* [GCC Make](https://gcc.gnu.org/onlinedocs/gccint/Makefile.html) - build tool

## Contributing

Currently a WIP, but make pull requests and I will approve ones I see as okay.

## Versioning

For the versions available, see the [tags on this repository](https://github.com/ZygalM1S1U/Mir/releases). 

## Authors

* **James Novorita** - *All Development* - [ZygalM1S1U](https://github.com/ZygalM1S1U), [Professional Website](https://zygalsplayground.com/)

## License

This project is licensed under the MIT License - see the [LICENSE.md](https://github.com/ZygalM1S1U/Mir/blob/master/LICENSE) file for details

## Acknowledgments

* Special thanks to Bryan for help with CMake and ImGui
