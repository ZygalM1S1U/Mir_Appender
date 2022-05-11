#ifndef _FILEIO_H__
#define _FILEIO_H__

#include "main.h"

typedef enum RECORD_TYPES_t
{
    DATA,
    END_OF_FILE_RECORD,
    EXTENDED_SEGMENT_ADDRESS,
    RFU_3,
    EXTENDED_LINEAR_ADDRESS,
    START_LINEAR_ADDRESS, // Only if it is an MDK-ARM variant
    NUM_TYPES
}RECORD_TYPES;

typedef enum MOTOROLA_RECORD_TYPES_t
{
    HEADER = 0,   // 16-bit address
    DATA_S1,      // 16-bit address
    DATA_S2,      // 24-bit address
    DATA_S3,      // 32-bit address
    RESERVED_S4,  // N/A
    COUNT_S5,     // 16-bit count
    COUNT_S6,     // 24-bit count
    START_ADDR_S7,// 32-bit address
    START_ADDR_S8,// 24-bit address
    START_ADDR_S9,// 16-bit address
    NUM_MOTO_RECORD_TYPES
}MOTOROLA_RECORD_TYPES;

/// @struct An intel hex file record field
typedef struct
{
    uint8_t recordSize;
    uint16_t extendedLinearAddress;
    uint16_t recordAddress;
    RECORD_TYPES recordType;
    uint32_t currentAddress;
    uint8_t data[16];
    uint8_t checksum;
}IntelHexRecordField;

typedef struct
{
    // Size of record
    uint8_t recordSize;

    // Count params
    uint32_t count;
    uint32_t countSize;

    // Address params
    uint16_t recordAddress;
    RECORD_TYPES recordType;
    uint32_t currentAddress;
    uint8_t data[16];
    uint8_t checksum;
}MotorolaSRecordField;

/// @struct A file struct after parsing
typedef struct
{
    char* fileName;
    IntelHexRecordField currentRecord;
    long fileSizeASCII;
    long fileSizeHex;
    uint8_t* intelHexFileASCII;
    uint16_t fileCRC;

}CurrentFileAttributes;

typedef struct
{
    char* fileName;
    MotorolaSRecordField currentRecord;
    long fileSizeASCII;
    long fileSizeHex;
    uint8_t* motorolaHexFileASCII;
    uint16_t fileCRC;

}CurrentMotoAttributes;


typedef struct
{
    FILE*   appendedFilePtr;
    long    appendedFileSize;
    long    appendIndex;
    fpos_t  appendLocation;
    char    crcToAppend[4];
    char    lengthToAppend[8];
    char    checksum[2];
}CrcAppendFileAttributes;

CurrentFileAttributes fileIntelHex;
CurrentMotoAttributes fileMotoHex;

/// @brief This function dumps an intel hex file into allocated memory.
/// @return A bool indicating success
bool dumpFile(char* fileString);

/// @brief This function dumps a Motorola SREC file into allocated memory.
/// @return A bool indicating success
bool dumpMotoFile(char* fileString);

/// @brief test for file print
void filePrint(void);

/// @brief Print the art
void printArt(void);

bool copyFileAndAppendFrame(char* sourceFileName, CrcAppendFileAttributes* crcFileInfo);

void writeFrame(CrcAppendFileAttributes* crcFileInfo, FILE* filePointer);

#endif // _FILEIO_H__
