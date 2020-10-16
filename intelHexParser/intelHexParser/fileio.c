#include "fileio.h"

void byteOrderHandling(char* frameString, CrcAppendFileAttributes* crcFileInfo);
void littleEndianSwap(char* frameString, CrcAppendFileAttributes* crcFileInfo);
void bigEndianSwap(char* frameString, CrcAppendFileAttributes* crcFileInfo);
void pdpEndianSwap(char* frameString, CrcAppendFileAttributes* crcFileInfo);
void honeywell316EndianSwap(char* frameString, CrcAppendFileAttributes* crcFileInfo);
void byteSwapSwapper(char* frameString, CrcAppendFileAttributes* crcFileInfo);

bool dumpFile(char* fileString)
{
    // Local Variables
    bool retVal = true;
    long hexFileSizeASCII = 0;

    // Local pointers
    FILE* fp;

#if DEBUG_ACTIVE
    printf("DEBUG : %s\n", fileString);
#endif // DEBUG_ACTIVE

    fileIntelHex.fileName = calloc(1, 25);

    // Copy the hex file
    memcpy(fileIntelHex.fileName, fileString, 25);

    // Open the file and check for errors
    fp = fopen(fileString, "rb");
    if(!fp) perror(fileString), exit(1);

    // Find the EOF and find the size
    fseek(fp, 0L, SEEK_END);
    hexFileSizeASCII = ftell(fp);

    // rewind the file pointer
    rewind(fp);

    // Allocate the entire file
    fileIntelHex.intelHexFileASCII = calloc(1, hexFileSizeASCII + 1);
    if(!fileIntelHex.intelHexFileASCII) fclose(fp), fputs("Memory allocation for the file failed", stderr), getchar(), exit(1);

    // Copy the entire file into a buffer
    if(1 != fread(fileIntelHex.intelHexFileASCII, hexFileSizeASCII, 1, fp))
        fclose(fp), free(fileIntelHex.intelHexFileASCII), fputs("Entire read fails", stderr), getchar(), exit(1);

    // Copy the ASCII size and dump the whole file, should already be allocated.
    fileIntelHex.fileSizeASCII = hexFileSizeASCII;

    // Close the file
    fclose(fp);

    // file has been dumped
    return retVal;

}

void filePrint(void)
{
    for(int i = 0; i < fileIntelHex.fileSizeASCII; ++i)
    {
        printf("%d", fileIntelHex.intelHexFileASCII[i]);
    }
}

void printArt(void)
{
    FILE* fptr;

    char filename[100] = "mirArt.txt", c;

    // Open file
    fptr = fopen(filename, "r");
    if (fptr == NULL)
    {
        printf("Cannot open file \n");
        exit(0);
    }

    // Read contents from file
    c = fgetc(fptr);
    while (c != EOF)
    {
        printf ("%c", c);
        c = fgetc(fptr);
    }

    fclose(fptr);
}

bool copyFileAndAppendFrame(char* sourceFileName, CrcAppendFileAttributes* crcFileInfo)
{
    FILE *source, *target;
    char ch;

    source = fopen(sourceFileName, "r");

#if DEBUG_ACTIVE
    printf("Source: %s\n", sourceFileName);
    printf("Target: %s\n", mir.fullFileOutputPath);
#endif

    if(source == NULL)
    {
       printf("Source File Read failure\n");
       exit(EXIT_FAILURE);
    }

    target = fopen(mir.fullFileOutputPath, "w");

    if(target == NULL)
    {
       fclose(source);
       printf("File write failed\n");
       exit(EXIT_FAILURE);
    }

#if DEBUG_ACTIVE
    printf("CRC location: %i\n",  crcFileInfo->appendIndex);
#endif

    // Append the CRC and length to the correct location
    while((ch = fgetc(source)) != EOF)
    {
        //printf("index: %i\n", index);

        // 16 is the offset to the end of the file
        if(ftell(target) == crcFileInfo->appendIndex)
        {
            // Build the string
            writeFrame(crcFileInfo, target);

            fseek(source, 18, SEEK_CUR);
        }
        else
        {
            fputc(ch, target);
        }
    }

    printf("File copied successfully.\n");

    fclose(source);
    fclose(target);
}

void writeFrame(CrcAppendFileAttributes* crcFileInfo, FILE* filePointer)
{
    /* So there is quite a bit going on here...
    * The whole idea for this particular function is to take all the information
    * that has been extracted from the intel hex file and place it in the correct
    * location in the new file, as an ASCII representation of the hex values.
    * 1. Convert the two 2-byte parameters we want to append (length and CRC) to hex for the record checksum computation
    * 2. Append the length of the record (which is always 8 bytes) to the first index of the checksum buffer followed by the address
    *    which is pulled from the configuration value from the config file.
    * 3. lay out the checksum buffer exactly how you want the ASCII file to be, based on your memory map for the checksum computation
    * 4. Calculate a checksum, in the MDK-ARM intel hex format, it is a one-byte sum of all data, followed by a modulo 256 operation,
    *    and finally, a two's complement of the data.  This completes the record to append.
    * 5. Convert the hex value of the checksum to ASCII and then build the ASCII string to write to the file.
    * 6. Finally, write the frame to the file.  This assumes the file pointer is already set to the correct location.
    */
    /// @todo ASSIGNED AS LITTLE ENDIAN
    /// @todo MAKE THIS CONFIGURABLE IN THE CONFIG FILE
    char frameString[18] = "";
    memset(frameString, '0', 18);

    // Compute the checksum and convert to ascii
    // Length will always be 8
    uint8_t checksumBuffer[12];
    uint8_t lengthHex[2];
    uint8_t crcHex[2];
    convASCIItoHex(crcFileInfo->crcToAppend, crcHex, 4);
    convASCIItoHex(crcFileInfo->lengthToAppend, lengthHex, 4);

    // Set all members of the buffer to 0, so that we only use instructions
    // for the things to be filled.
    memset(checksumBuffer, 0u, 12);

    // Appending the length
    checksumBuffer[0] = 0x08;

    // Pull the address from the bottom 16 bits
    uint16_t address = (mir.appendageAddressFile & 0x0000FFFF);

    // address packing
    checksumBuffer[1] = (address & 0xFF00)>>8;
    checksumBuffer[2] = address & 0x00FF;

    // Length packing
    checksumBuffer[4] = lengthHex[0];
    checksumBuffer[5] = lengthHex[1];

    // Crc packing
    checksumBuffer[6] = crcHex[0];
    checksumBuffer[7] = crcHex[1];

#if DEBUG_ACTIVE
    for(int i = 0; i < 12; ++i)
        printf("0x%02X\n", checksumBuffer[i]);

    printNewLine();
#endif

    uint8_t checksum = 0u;

    // Calculate the 256% 2's complement checksum
    checksum = checksum2sComp(checksumBuffer, sizeof(checksumBuffer));

#if DEBUG_ACTIVE
    printf("Checksum: 0x%02X\n", checksum);
#endif

    // Finish the frame by converting the final byte to ASCII
    convHex2ASCII(checksum, crcFileInfo->checksum);

    // Swap the bytes accordingly
    byteOrderHandling(frameString, crcFileInfo);

    // Write the string built to the file
    fwrite(frameString, 1, 18, filePointer);
}

void byteOrderHandling(char* frameString, CrcAppendFileAttributes* crcFileInfo)
{
    switch(mir.byteOrder)
    {
    case MIR_LITTLE_ENDIAN:
        littleEndianSwap(frameString, crcFileInfo);
        break;
    case MIR_BIG_ENDIAN:
        bigEndianSwap(frameString, crcFileInfo);
        break;
    case MIR_PDP_ENDIAN:
        pdpEndianSwap(frameString, crcFileInfo);
        break;
    case MIR_HONEYWELL316_ENDIAN:
        honeywell316EndianSwap(frameString, crcFileInfo);
        break;
    case MIR_BYTE_SWAPPED:
        byteSwapSwapper(frameString, crcFileInfo);
        break;
    default:
        /// @note Defaults to little endian
        littleEndianSwap(frameString, crcFileInfo);
        break;
    }
}

void littleEndianSwap(char* frameString, CrcAppendFileAttributes* crcFileInfo)
{
#if    DEBUG_ACTIVE
    printf("Little Endian storage\n");
#endif
    // Build the string of the frame to append
    frameString[0] = crcFileInfo->lengthToAppend[2];
    frameString[1] = crcFileInfo->lengthToAppend[3];
    frameString[2] = crcFileInfo->lengthToAppend[0];
    frameString[3] = crcFileInfo->lengthToAppend[1];
    frameString[8] = crcFileInfo->crcToAppend[2];
    frameString[9] = crcFileInfo->crcToAppend[3];
    frameString[10] = crcFileInfo->crcToAppend[0];
    frameString[11] = crcFileInfo->crcToAppend[1];
    frameString[16] = crcFileInfo->checksum[0];
    frameString[17] = crcFileInfo->checksum[1];
}

void bigEndianSwap(char* frameString, CrcAppendFileAttributes* crcFileInfo)
{
#if    DEBUG_ACTIVE
    printf("Big Endian storage\n");
#endif
    // Build the string of the frame to append
    frameString[0] = crcFileInfo->lengthToAppend[0];
    frameString[1] = crcFileInfo->lengthToAppend[1];
    frameString[2] = crcFileInfo->lengthToAppend[2];
    frameString[3] = crcFileInfo->lengthToAppend[3];
    frameString[8] = crcFileInfo->crcToAppend[0];
    frameString[9] = crcFileInfo->crcToAppend[1];
    frameString[10] = crcFileInfo->crcToAppend[2];
    frameString[11] = crcFileInfo->crcToAppend[3];
    frameString[16] = crcFileInfo->checksum[0];
    frameString[17] = crcFileInfo->checksum[1];
}

void pdpEndianSwap(char* frameString, CrcAppendFileAttributes* crcFileInfo)
{
#if    DEBUG_ACTIVE
    printf("PDP Endian storage\n");
#endif
    frameString[0] = crcFileInfo->lengthToAppend[2];
    frameString[1] = crcFileInfo->lengthToAppend[3];
    frameString[2] = crcFileInfo->lengthToAppend[0];
    frameString[3] = crcFileInfo->lengthToAppend[1];
    frameString[8] = crcFileInfo->crcToAppend[2];
    frameString[9] = crcFileInfo->crcToAppend[3];
    frameString[10] = crcFileInfo->crcToAppend[0];
    frameString[11] = crcFileInfo->crcToAppend[1];
    frameString[16] = crcFileInfo->checksum[0];
    frameString[17] = crcFileInfo->checksum[1];
}

void honeywell316EndianSwap(char* frameString, CrcAppendFileAttributes* crcFileInfo)
{
#if    DEBUG_ACTIVE
    printf("Honeywell 316 storage\n");
#endif
    // Build the string of the frame to append
    frameString[8] = crcFileInfo->lengthToAppend[0];
    frameString[9] = crcFileInfo->lengthToAppend[1];
    frameString[10] = crcFileInfo->lengthToAppend[2];
    frameString[11] = crcFileInfo->lengthToAppend[3];
    frameString[0] = crcFileInfo->crcToAppend[0];
    frameString[1] = crcFileInfo->crcToAppend[1];
    frameString[2] = crcFileInfo->crcToAppend[2];
    frameString[3] = crcFileInfo->crcToAppend[3];
    frameString[16] = crcFileInfo->checksum[0];
    frameString[17] = crcFileInfo->checksum[1];
}

void byteSwapSwapper(char* frameString, CrcAppendFileAttributes* crcFileInfo)
{
#if    DEBUG_ACTIVE
    printf("Byte swapped storage\n");
#endif
    frameString[0] = crcFileInfo->lengthToAppend[2];
    frameString[1] = crcFileInfo->lengthToAppend[3];
    frameString[2] = crcFileInfo->lengthToAppend[0];
    frameString[3] = crcFileInfo->lengthToAppend[1];
    frameString[8] = crcFileInfo->crcToAppend[2];
    frameString[9] = crcFileInfo->crcToAppend[3];
    frameString[10] = crcFileInfo->crcToAppend[0];
    frameString[11] = crcFileInfo->crcToAppend[1];
    frameString[16] = crcFileInfo->checksum[0];
    frameString[17] = crcFileInfo->checksum[1];
}
