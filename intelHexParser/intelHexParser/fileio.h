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
    FILE*   appendedFilePtr;
    long    appendedFileSize;
    long    appendIndex;
    fpos_t  appendLocation;
    char    crcToAppend[4];
    char    lengthToAppend[4];
    char    checksum[2];
}CrcAppendFileAttributes;

CurrentFileAttributes fileIntelHex;

/// @brief This function dumps a file into allocated memory.
/// @return A bool indicating success
bool dumpFile(char* fileString);

/// @brief test for file print
void filePrint(void);

/// @brief Print the art
void printArt(void);

bool copyFileAndAppendFrame(char* sourceFileName, CrcAppendFileAttributes* crcFileInfo);

void writeFrame(CrcAppendFileAttributes* crcFileInfo, FILE* filePointer);

#endif // _FILEIO_H__
