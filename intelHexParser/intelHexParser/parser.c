#include "parser.h"

/** @brief This is the size in ASCII of the padding inside the record
           since we slide after the :, it starts at the size
           10 0000, So this then starts at record type.
*/
#define INTEL_HEX_RECORD_PAD_SIZE    6
#define INTEL_HEX_FILE_APPEND_OFFSET 19
CrcAppendFileAttributes crcFileInformation;

/// @section Motorola Delimiters
#define MOTOROLA_SREC_TYPE_DELIMITER 0
#define MOTOROLA_SREC_SIZE_DELIMITER 2


char srecTextMap[NUM_MOTO_RECORD_TYPES][20] = {
    "HEADER = 0",   // 16-bit address
    "DATA_S1",      // 16-bit address
    "DATA_S2",      // 24-bit address
    "DATA_S3",      // 32-bit address
    "RESERVED_S4",  // N/A
    "COUNT_S5",     // 16-bit count
    "COUNT_S6",     // 24-bit count
    "START_ADDR_S7",// 32-bit address
    "START_ADDR_S8",// 24-bit address
    "START_ADDR_S9",// 16-bit address
    };


/// @section Private prototypes
void clearFile(void);
void calculateCRCBinaryBlob(void);
void recordTypeHandler(RECORD_TYPES recordType, uint8_t* data);
void appendCRC(void);
void extractAndStoreHexFromRecords(void);
void decipherSRecord(uint16_t lineIndex, long currentFileIndex);
void motoRecordTypeHandler(MOTOROLA_RECORD_TYPES recordType, char* lineBuffer, long lineSize);
void motoRecordHeaderHandler(char* lineBuffer, long lineSize);

bool fileParsing = true;
bool foundAppendAddress = false;
PARSE_STATES stateReturn = FILE_IO;
extern CurrentFileAttributes fileIntelHex;
extern hexFile intelHexFile;
uint8_t debugStuff = 0;
uint64_t size = 0;

void parse(char* hexFilePath)
{
    clock_t start_t;
    // Start the state machine for the parsing
    while(fileParsing)
    {
        switch(stateReturn)
        {
        case FILE_IO:
            // Dump the file into a memory pool.
            // If it successful, start parsing.
            if(dumpFile(hexFilePath))
            {
                stateReturn = FILE_EXTRACT;
            }
            break;
        case FILE_PARSE:
            //printf("You have reached the parsing stage\n");
            filePrint();
            break;
        case FILE_EXTRACT:
            //printf("File extract\n");
            start_t = clock();
            stateReturn = extractRecord();
            break;
        case DATA_STORE:
            stateReturn = FILE_IO;
            fileParsing = false;
            break;
        case CALC_CRC:
            calculateCRCBinaryBlob();
            stateReturn = APPEND_CRC;
            break;
        case APPEND_CRC:
            // Append the CRC to the file after copying
            if(foundAppendAddress)
            {
                printf("File size: 0x%08X  File CRC: 0x%04X\n", fileIntelHex.fileSizeHex, fileIntelHex.fileCRC);
                mir.lastKnownExecutionTime = 1000*((double)(clock() - start_t)/ CLOCKS_PER_SEC);
                printf("Total execution time: %f ms\n", mir.lastKnownExecutionTime);
                appendCRC();
            }
            else
            {
                // File has no valid record
                printf("File has no valid record for appendage based on settings.\nCheck the mirConfig file for the Append record\n");
            }
            clearFile();
            stateReturn = FILE_IO;
            fileParsing = false;
            break;
        case HALT:
            // Halt the file parsing task
            stateReturn = FILE_IO;
            fileParsing = false;
            break;
        default:
            break;
        }
    }

    fileParsing = true;
}

PARSE_STATES extractRecord(void)
{
    // Start the extraction by reading the first record
    // 48 is the size in ASCII of the max record.  Will get cleared each time.
    uint8_t* tempReadBuffer = malloc(1000/sizeof(uint8_t));
    uint8_t* hexBuffer = malloc(50/sizeof(uint8_t));
    uint8_t sizeBuffer[5] = "";
    uint8_t* lineBuffer = malloc(100/sizeof(uint8_t));

    // Allocate heap space for the hexfile mass
    intelHexFile.hexFileBuffer = calloc(1, (fileIntelHex.fileSizeASCII/2));
    memset(tempReadBuffer, 0u, sizeof(tempReadBuffer));
    memset(hexBuffer, 0u, sizeof(hexBuffer));
    memset(lineBuffer, 0u, sizeof(lineBuffer));
    uint32_t lineCount = 1;
    char lineBeginDelimiter = 58;
    uint16_t lineIndex = 0;
    uint8_t recordSizeASCII[2] = "";
    bool lineFlag = false;
    int lineCounter = 0;

    // log stats of each lineifdef
#if DEBUG_ACTIVE
    printf("fileSize in ASCII: %ld\n", fileIntelHex.fileSizeASCII);
#endif

    for(long i = 0; i < fileIntelHex.fileSizeASCII; ++i)
    {
        // Copy everything into a buffer
        tempReadBuffer[lineIndex] = fileIntelHex.intelHexFileASCII[i];

        debugStuff = i;
        if(fileIntelHex.intelHexFileASCII[i] == lineBeginDelimiter)
        {
            uint8_t* lineBuffer = calloc(1, 80);
            uint8_t* hexBuffer = calloc(1, 50);
            uint8_t* tempReadBuffer = calloc(1, 60);
            // First index after a return is always :, slide by 1
            ++lineIndex;
            lineFlag = true;
        }
#if DEBUG_ACTIVE
        // Debug purposes....
        printf("%c", fileIntelHex.intelHexFileASCII[i]);
#endif

        // If the line flag is present, store data in the line buffer
        if(lineFlag)
        {
            lineBuffer[lineIndex] = tempReadBuffer[lineIndex];
        }

        // Copy the record size into the record struct
        if(lineIndex == RECORD_SIZE_DELIMITER)
        {
            // store the record size
            recordSizeASCII[0] = fileIntelHex.intelHexFileASCII[i+1];
            recordSizeASCII[1] = fileIntelHex.intelHexFileASCII[i+2];
            convASCIItoHex(recordSizeASCII, sizeBuffer, 2);
            // Slam the array into a u8, since we know records can only be 16 bytes
            fileIntelHex.currentRecord.recordSize = *(uint8_t*)sizeBuffer;
            if(fileIntelHex.currentRecord.recordSize == 0x00)
            {
                // File over, free the dynamic memory
#if DEBUG_ACTIVE
                printf("End of file\n");
                // End of file
                printf("Current file size: 0x%08X\n", fileIntelHex.fileSizeHex);
#endif
                // CLEAR EVERYTHING
                lineFlag = false;
                free(lineBuffer);
                free(tempReadBuffer);
                free(hexBuffer);
                return CALC_CRC;
            }
        }

        //Check for the end of line
        if(fileIntelHex.intelHexFileASCII[i] == '\n')
        {
            // Increment the line counter
            ++lineCount;
#if DEBUG_ACTIVE
            printf("Line count: %d\n", lineCount);
#endif

            // Reset the line index
            lineIndex = 0;
#if DEBUG_ACTIVE
            printf("Line buffer contents ASCII: ");
            for(int i= 0; i < (fileIntelHex.currentRecord.recordSize*2)+12; ++i)
                printf("%c", lineBuffer[i]);
            printNewLine();
#endif

            // Make the data useful first ;)
            convASCIItoHex(lineBuffer, hexBuffer, (fileIntelHex.currentRecord.recordSize*2)+11);
#if DEBUG_ACTIVE
            printf("Line buffer contents hex: ");
            for(int i= 0; i < fileIntelHex.currentRecord.recordSize+6; ++i)
                printf("0x%02X ", hexBuffer[i]);
            printNewLine();
#endif

            // Copy the line into the record struct buffer
            analyzeRecord(hexBuffer, fileIntelHex.currentRecord.recordSize);

            // Check for the end of line
            if(fileIntelHex.currentRecord.currentAddress == mir.appendageAddressFile)
            {
                crcFileInformation.appendIndex = i - INTEL_HEX_FILE_APPEND_OFFSET;
#if DEBUG_ACTIVE
                printf("Appendage address found\n");
#endif
                foundAppendAddress = true;
            }

#if DEBUG_ACTIVE
            printf("Size: 0x%08X\n", fileIntelHex.fileSizeHex);
#endif
            // Reset all the line parameters
            memset(tempReadBuffer, 0u, sizeof(tempReadBuffer));
            memset(lineBuffer, 0u, sizeof(lineBuffer));
            memset(hexBuffer, 0u, sizeof(hexBuffer));
            lineFlag = false;

            // Debug stuff
            ++lineCounter;
        }
        if(lineIndex)
        {
            ++lineIndex;
        }
    }

    return CALC_CRC;
}

void analyzeRecord(uint8_t* recordBuffer, uint8_t recordSize)
{
    uint8_t* parseSizeTypeChksmBuffer = calloc(1, 20);
    uint8_t* parseAddressBuffer = calloc(1, 20);
    uint8_t* dataBuffer = calloc(1, 20);
    int addressBufferIndex = 0;
    int typeIndex = 0;
    int recordIndex = 0;
    int dataIndex = 0;

#if DEBUG_ACTIVE
    printf("Hex Record: ");
#endif

    while(recordIndex != (recordSize+INTEL_HEX_RECORD_PAD_SIZE))
    {
        if(recordIndex < HEX_RECORD_SIZE)
        {
            // Size is already noted in the file struct
        }
        else if(recordIndex >= HEX_RECORD_SIZE && recordIndex < HEX_RECORD_ADDRESS)
        {
            parseAddressBuffer[addressBufferIndex] = recordBuffer[recordIndex+1];
            ++addressBufferIndex;

        }
        else if(recordIndex >= (HEX_RECORD_ADDRESS+1) && recordIndex < (HEX_RECORD_TYPE - 1))
        {
            parseSizeTypeChksmBuffer[typeIndex] = recordBuffer[recordIndex];
            ++typeIndex;
            if(*(uint16_t*)parseSizeTypeChksmBuffer == END_OF_FILE_RECORD)
                fileIntelHex.currentRecord.recordType = END_OF_FILE_RECORD;
        }
        else if(recordIndex >= HEX_RECORD_TYPE - 1)
        {
            // Just data
            dataBuffer[dataIndex] = recordBuffer[recordIndex];
            ++dataIndex;
        }

        recordStore(recordIndex, parseSizeTypeChksmBuffer, parseAddressBuffer);
        ++recordIndex;
    }

    // Copy the data from the record into the struct using the record size
    memcpy(&fileIntelHex.currentRecord.data, dataBuffer, sizeof(recordSize));
    recordTypeHandler(fileIntelHex.currentRecord.recordType, dataBuffer);

#if DEBUG_ACTIVE
    printf("Data Buffer:");
    for(int i= 0; i < fileIntelHex.currentRecord.recordSize; ++i)
        printf("0x%02X ", dataBuffer[i]);
    printNewLine();

    printf("File Size Before fill: 0x%08X\n", fileIntelHex.fileSizeHex);
#endif

    if(fileIntelHex.currentRecord.recordType == DATA
            && fileIntelHex.currentRecord.currentAddress != mir.appendageAddressFile)
    {
        // Fill the CRC calc buffer with the record data and increment by the current record size
        memcpy(&intelHexFile.hexFileBuffer[fileIntelHex.fileSizeHex], dataBuffer, fileIntelHex.currentRecord.recordSize);

        // Increment the size
        fileIntelHex.fileSizeHex += fileIntelHex.currentRecord.recordSize;
    }

#if DEBUG_ACTIVE
    printNewLine();
#endif

    free(parseSizeTypeChksmBuffer);
    free(parseAddressBuffer);
    free(dataBuffer);
}

void recordStore(uint8_t tempSlidePass, uint8_t* sizeTypeChksmhexBuffer, uint8_t* addressHexBuffer)
{
    uint16_t testAddress = 0u;

    //Categorize the hex record by copying its size, address, and type to the global struct for the current record of the file.  This way it can be accessed
    switch (tempSlidePass)
    {
    case HEX_RECORD_SIZE:
#if  DEBUG_ACTIVE
        printf("Record Size: 0x%02X\n", fileIntelHex.currentRecord.recordSize);
#endif
        break;
    case HEX_RECORD_ADDRESS:
        // slam two bytes into a 16-bit unsigned
        fileIntelHex.currentRecord.recordAddress = ((addressHexBuffer[0]<<8) | addressHexBuffer[1]);
        testAddress = *(uint16_t*)addressHexBuffer;
        fileIntelHex.currentRecord.currentAddress = ((fileIntelHex.currentRecord.extendedLinearAddress<<16)|(fileIntelHex.currentRecord.recordAddress));
#if  DEBUG_ACTIVE
        printf("Current address: 0x%08X\n", fileIntelHex.currentRecord.currentAddress);
#endif
        memcpy(&fileIntelHex.currentRecord.recordAddress, &testAddress, sizeof(fileIntelHex.currentRecord.recordAddress));
        memset(addressHexBuffer, 0, sizeof(&addressHexBuffer));
        break;
    case HEX_RECORD_TYPE:
        memcpy(&fileIntelHex.currentRecord.recordType, sizeTypeChksmhexBuffer, sizeof(fileIntelHex.currentRecord.recordType));
#if     DEBUG_ACTIVE
        printf("Record Type: 0x%02X - ", fileIntelHex.currentRecord.recordType);
#endif
        memset(sizeTypeChksmhexBuffer, 0, sizeof(&sizeTypeChksmhexBuffer));
        break;
    default:
        ///Do nothing...
        break;
    }
}

void clearFile(void)
{
    // Clear the file struct
    foundAppendAddress = false;

    //Global file params
    fileIntelHex.fileSizeASCII = 0;
    fileIntelHex.fileSizeHex = 0;
    memset(fileIntelHex.intelHexFileASCII, 0u, sizeof(fileIntelHex.intelHexFileASCII));

    // Final record params
    fileIntelHex.currentRecord.checksum = 0u;
    fileIntelHex.currentRecord.currentAddress = 0u;
    fileIntelHex.currentRecord.extendedLinearAddress = 0u;
    fileIntelHex.currentRecord.recordAddress = 0u;
    fileIntelHex.currentRecord.recordSize = 0u;
    memset(fileIntelHex.currentRecord.data, 0u, sizeof(fileIntelHex.currentRecord.data));

}

void calculateCRCBinaryBlob(void)
{
    // Calculate the CRC for the file
    fileIntelHex.fileCRC = (uint16_t)(*mir.crcSetting.crcCalculateFunc)(intelHexFile.hexFileBuffer, fileIntelHex.fileSizeHex);
#if DEBUG_ACTIVE
    printf("File size: 0x%08X  File CRC: 0x%04X\n", fileIntelHex.fileSizeHex, fileIntelHex.fileCRC);
#endif

    uint8_t crc[2];
    crc[0] = fileIntelHex.fileCRC >> 8;
    crc[1] = fileIntelHex.fileCRC & 0x00FF;
#if DEBUG_ACTIVE
    printf("First nibble: 0x%04X\n", crc[0]);
#endif

    uint8_t length[4];
    length[0] = fileIntelHex.fileSizeHex >> 24;
    length[1] = fileIntelHex.fileSizeHex >> 16;
    length[2] = (fileIntelHex.fileSizeHex & 0x0000FF00) >> 8;
    length[3] = (uint8_t)(fileIntelHex.fileSizeHex & 0x000000FF);

    char crcChar[4] = "";
    char lengthChar[8] = "";

    // Printing the length and CRC in ASCII

    // Convert the Hex to ASCII
    convHex2ASCII(crc[0], crcChar);
    convHex2ASCII(length[0], lengthChar);

    // Fill the buffers
    crcFileInformation.crcToAppend[0] = crcChar[0];
    crcFileInformation.crcToAppend[1] = crcChar[1];
    crcFileInformation.lengthToAppend[0] = lengthChar[0];
    crcFileInformation.lengthToAppend[1] = lengthChar[1];

    // Convert the Hex to ASCII
    convHex2ASCII(crc[1], crcChar);
    convHex2ASCII(length[1], lengthChar);

    // Fill the buffers
    crcFileInformation.crcToAppend[2] = crcChar[0];
    crcFileInformation.crcToAppend[3] = crcChar[1];
    crcFileInformation.lengthToAppend[2] = lengthChar[0];
    crcFileInformation.lengthToAppend[3] = lengthChar[1];

    convHex2ASCII(length[2], lengthChar);
    crcFileInformation.lengthToAppend[4] = lengthChar[0];
    crcFileInformation.lengthToAppend[5] = lengthChar[1];
    convHex2ASCII(length[3], lengthChar);
    crcFileInformation.lengthToAppend[6] = lengthChar[0];
    crcFileInformation.lengthToAppend[7] = lengthChar[1];

#if DEBUG_ACTIVE
    printf("CRC ASCII: ");
    for(int i = 0; i < 4; ++i)
        printf("%c", crcFileInformation.crcToAppend[i]);

    printNewLine();

    printf("LENGTH ASCII: ");
    for(int i = 0; i < 8; ++i)
        printf("%c", crcFileInformation.lengthToAppend[i]);

    printNewLine();
#endif
}

void recordTypeHandler(RECORD_TYPES recordType, uint8_t* data)
{
    switch(recordType)
    {
    case DATA:
        // Don't do jack
#if DEBUG_ACTIVE
        printf("Data\n");
#endif
        break;
    case END_OF_FILE_RECORD:
#if DEBUG_ACTIVE
        printf("End of file\n");
#endif
        break;
    case EXTENDED_LINEAR_ADDRESS:
        // If not using H167 directive
#if DEBUG_ACTIVE
        printf("Extended Linear Address\n");
#endif
        fileIntelHex.currentRecord.extendedLinearAddress = (data[0]<<8) | data[1];
        fileIntelHex.currentRecord.currentAddress = ((fileIntelHex.currentRecord.extendedLinearAddress<<16)
                                                     | (fileIntelHex.currentRecord.recordAddress));
        break;
    case START_LINEAR_ADDRESS:
#if DEBUG_ACTIVE
        printf("Start Linear Address\n");
#endif
        break;
    case EXTENDED_SEGMENT_ADDRESS:
        // If not using the H167 directive
        fileIntelHex.currentRecord.extendedLinearAddress = (data[0]<<8) | data[1];
        fileIntelHex.currentRecord.currentAddress = ((fileIntelHex.currentRecord.extendedLinearAddress<<16)
                                                     | (fileIntelHex.currentRecord.recordAddress));
#if DEBUG_ACTIVE
        printf("Start Segment Address Record\n");
#endif
        break;
    default:
        printf("UNHANDLED RECORD TYPE!!\n");
        break;
    }
}

void appendCRC(void)
{
    // Copy the file into the new place for the appendage
    copyFileAndAppendFrame(mir.fullFileInputPath, &crcFileInformation);
}

void parseSREC(char* hexFilePath)
{
    clock_t start_t;

    if(dumpMotoFile(hexFilePath))
    {
        start_t = clock();

        extractAndStoreHexFromRecords();
    }
}

void extractAndStoreHexFromRecords(void)
{
    bool lineBeginFlag = false;
    uint16_t lineIndex = 0u;
    long lineCount = 0u;

#if DEBUG_ACTIVE
    printf("File size ASCII: %d", fileMotoHex.fileSizeASCII);
    printNewLine();
#endif

    for(long i = 0; i < fileMotoHex.fileSizeASCII; ++i)
    {
        // Start looking
        if(fileMotoHex.motorolaHexFileASCII[i] == 83 && lineBeginFlag == false)
        {
            lineBeginFlag = true;
        }

        decipherSRecord(lineIndex, i);

        if(lineBeginFlag)
            ++lineIndex;

        // Check for the end of line
        if(fileMotoHex.motorolaHexFileASCII[i] == '\n')
        {
            lineBeginFlag = false;
#if DEBUG_ACTIVE

        printf("Line Count: %u\n", lineCount++);
#endif
            lineIndex = 0u;
        }
    }
}

void decipherSRecord(uint16_t lineIndex, long currentFileIndex)
{
    static long fileSizeHold;

    if(lineIndex == MOTOROLA_SREC_SIZE_DELIMITER-1)
    {
        char recordTypeASCII[2] = "";
        uint8_t typeBuffer[1] = "";

        // store the record Type, S becomes a 0
        recordTypeASCII[0] = '0';
        recordTypeASCII[1] = fileMotoHex.motorolaHexFileASCII[currentFileIndex];

#if DEBUG_ACTIVE
        // Save the FP Index
        fileSizeHold = currentFileIndex;
#endif

        convASCIItoHex(recordTypeASCII, typeBuffer, 2);

        fileMotoHex.currentRecord.recordType = *(uint8_t*)typeBuffer;

#if DEBUG_ACTIVE
        printf("Record Type ASCII: %c%c ", fileMotoHex.motorolaHexFileASCII[currentFileIndex], fileMotoHex.motorolaHexFileASCII[currentFileIndex+1]);
        printf("Record Type hex: 0x%04X - ", fileMotoHex.currentRecord.recordType);
        printf("%s\n", srecTextMap[fileMotoHex.currentRecord.recordType]);
#endif
    }
    else if(lineIndex == MOTOROLA_SREC_SIZE_DELIMITER)
    {
        char recordSizeASCII[2] = "";
        uint8_t sizeBuffer[1] = "";

        // store the record Type, S becomes a 0
        recordSizeASCII[0] = fileMotoHex.motorolaHexFileASCII[currentFileIndex];;
        recordSizeASCII[1] = fileMotoHex.motorolaHexFileASCII[currentFileIndex];

        convASCIItoHex((uint8_t*)&fileMotoHex.motorolaHexFileASCII[currentFileIndex], sizeBuffer, 2);

        fileMotoHex.currentRecord.recordSize = *(uint8_t*)sizeBuffer;

#if DEBUG_ACTIVE
        printf("Record Size ASCII: %c%c ", fileMotoHex.motorolaHexFileASCII[currentFileIndex], fileMotoHex.motorolaHexFileASCII[currentFileIndex]);
        printf("Record Size hex: 0x%02X\n", fileMotoHex.currentRecord.recordSize);
        printf("Line Contents ASCII: ");
        for(int j = 0; j < ((fileMotoHex.currentRecord.recordSize*2)+2); ++j)
            printf("%c", fileMotoHex.motorolaHexFileASCII[currentFileIndex+j]);
        printNewLine();

#endif
    }
}


void motoRecordTypeHandler(MOTOROLA_RECORD_TYPES recordType, char* lineBuffer, long lineSize)
{
    switch(recordType)
    {
    case HEADER:
        motoRecordHeaderHandler(lineBuffer, lineSize);
        break;
    case DATA_S1:
        break;
    case DATA_S2:
        break;
    case DATA_S3:
        break;
    case COUNT_S5:
        break;
    case COUNT_S6:
        break;
    case START_ADDR_S7:
        break;
    case START_ADDR_S8:
        break;
    case START_ADDR_S9:
        break;
    case RESERVED_S4:
    default:
        break;
    }
}

void motoRecordHeaderHandler(char* lineBuffer, long lineSize)
{

}

