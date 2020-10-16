#ifndef _UTILS_H__
#define _UTILS_H__

#include "main.h"

/// @struct dynamic memory, dynamically sized array
typedef struct
{
    int* array;
    size_t used;
    size_t size;
}Array;

/// @brief This function will check to see if malloc works ;)
/// @return a return code
bool memCheck(void);

/// @brief initialize the array with the size
/// @note pass the size from the file into the initial size parameter
void initArray(Array* a, size_t initialSize);

/// @brief A function to re-allocate the array previously allocated when the index is reached
/// @note Invoke the same way as function: initArray
void reallArray(Array* a, size_t initialSize);

/// @brief A function to insert the array at a specified element
void insertArray(Array* a, int element);

/// @brief A function to free the array set.
void freeArray(Array* a);

/// @brief Hex to ASCII
void convASCIItoHex(uint8_t* ASCIIBuffer, uint8_t* hexBufferPacked, uint64_t size);

/// @brief Calculate CRC-16 CCITT with 0xFFFF seed and a 0x1021 polynomial
/// @return the calculated CRC
uint16_t crcCCITT16Calculate(const uint8_t* dataBuffer, const long size);

/// @brief calculates a CRC-16 mod x checksum
uint16_t crc16XModCalc(const uint8_t* data, const long size);

/// @brief A function for calculating a 16-bit CRC for arbitrary data
/// @returns the calculated crc
uint16_t crcIEC60335Calculate(const uint8_t* data, const long size);

/// @brief Swap endianness of two bytes
/// @return a swapped endian order two-byte value
uint16_t swapEndianness(uint8_t* data);

/// @brief will convert hex to ACSII
void convHex2ASCII(uint8_t source, char* dest);

/// @brief Lookup table for ASCII
char lookupTableASCII(uint8_t nibble);

/// @brief Twos Complement Checksum for frame appendage
/// @return The resultant of the checksum
uint8_t checksum2sComp(uint8_t* data, long size);

/// @brief a general function to print strings throughout the program
void printString(char* string, long size);

/// @brief Print a new line using printf
void printNewLine(void);

#endif // _UTILS_H__
