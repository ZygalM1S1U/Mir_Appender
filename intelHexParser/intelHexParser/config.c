#include "config.h"
#include "dirent.h"

/// @section Private Function Prototypes
/// @brief Parse the config file for mir config settings
bool parseConfigFile(void);

/// @brief Retrieve the fields from the file
void retrieveFields(FILE* fp);

/// @brief Find sequences in the config file
void decipherInfo(uint8_t* info, uint8_t lineCount);

/// @brief Get the record information from the record itself in the file
void getRecordInformation(uint8_t* info, uint8_t* infoToBeStored, uint8_t lineIndex);

uint8_t infoBuffer[24] = "";

static struct asciiConfigSettings_t 
{
    char operatingMode[2];            /// @note this needs to changed if there are more than 9 op modes
    char appendAddress[16];			  /// @note This needs to be changed for anything larger than 64 bit addressing
    char crcFunctionPtr[2];           /// @note this needs to changed if there are more than 9 function pointers
    char byteOrder[2];
    char fileLocation[MAX_STR_LEN];
    char fileName[MAX_STR_LEN];       /// @note this must be changed if the string is longer than 24
    char outputLocation[MAX_STR_LEN];
    char outputFileName[MAX_STR_LEN];
    char fileType[2];
    uint8_t sizes[NUM_OF_MIR_RECORD_TYPES];
}asciiConfigSettings;

/// @section Function pointer array for crc functions
uint16_t (*crcCalculateFuncs[NUM_OF_CRC_FUNCS])(const uint8_t* data, const long size) =
         {
            crcCCITT16Calculate,
            crc16XModCalc,
            crcIEC60335Calculate,
         };

/// @section String maps for debugging
#if    DEBUG_ACTIVE
static char confCRCFields[NUM_OF_CRC_FUNCS][MIR_CONFIG_RECORD_MAX_SIZE]  = 
{
    "CRC16CCIT",
    "CRC16_X_MOD",
    "CRC16_IEC_60335",
};

static char confModeFields[MIR_NUM_MODES][MIR_CONFIG_RECORD_MAX_SIZE]  = 
{
    "MIR_APPENDER_MODE",
    "MIR_UI_MODE",
    "MIR_TEST_MODE"
};

static char confByteFields[MIR_NUM_OF_BYTE_ORDER][MIR_CONFIG_RECORD_MAX_SIZE]  = 
{
    "MIR_LITTLE_ENDIAN",
    "MIR_BIG_ENDIAN",
    "MIR_MIDDLE_ENDIAN",
    "MIR_PDP_ENDIAN",
    "MIR_HONEYWELL316_ENDIAN",
    "MIR_BYTE_SWAPPED"
};
#endif


bool parseConfigFile(void)
{
    DIR* dir;
    FILE* fptr;
    bool retVal = false;

    // Open the config file 
    // Set the current directory of the file
    /// @note this should be noted, as this will break the program
    // Open the config file
    fptr = fopen("scripts/mirConfig.mir", "r");
    if(fptr == NULL)
    {
        printf("Cannot open config file \n");
        exit(0);
    }
    else
    {
#if     DEBUG_ACTIVE
        printf("config file opened\n");
#endif
        retrieveFields(fptr);
        retVal = true;
    }
    
    return retVal;
}

bool retrieveConfigurationSettings(void)
{
    bool retVal = true;

    // We want to pull the CRC settings from a config file
    retVal = parseConfigFile();

    /// @todo pull this from the config file
    /// @note hack for now
    uint8_t appendAddress[9] = "";
    convASCIItoHex((uint8_t*)asciiConfigSettings.appendAddress, appendAddress, sizeof(asciiConfigSettings.appendAddress));
    mir.appendageAddressFile = appendAddress[0] << 24 | appendAddress[1] << 16  | appendAddress[2] << 8 | appendAddress[3];

#if    DEBUG_ACTIVE
    printNewLine();
    printf("Append Address: ");
    printf("0x%08X ", mir.appendageAddressFile);
    printNewLine();
#endif

    // Assign the CRC function pointer
    /// @todo pull the index from the config file, hack for now
    uint8_t crcFunc[1] = "";
    convASCIItoHex((uint8_t*)asciiConfigSettings.crcFunctionPtr, crcFunc, 2);
    mir.crcSetting.crcFuncPtrIndex = crcFunc[0];
    mir.crcSetting.crcCalculateFunc = crcCalculateFuncs[crcFunc[0]];

#if    DEBUG_ACTIVE
    printf("CRC: ");
    printf("0x%02X ", crcFunc[0]);
    printNewLine();
#endif

    // Assign the mode
    /// @todo, make this in the config file, hack for now
    uint8_t opMode[1] = "";
    convASCIItoHex((uint8_t*)asciiConfigSettings.operatingMode, opMode, 2);
    mir.mirMode = opMode[0];

#if    DEBUG_ACTIVE
    printf("Mir Mode: ");
    printf("0x%02X ", mir.mirMode);
    printNewLine();
#endif

    // Assign the byte order
	/// @todo HACK FOR NOW
    uint8_t byteOrder[2] = "";
    convASCIItoHex((uint8_t*)asciiConfigSettings.byteOrder, byteOrder, 2);
    mir.byteOrder = byteOrder[0];

#if    DEBUG_ACTIVE
    printf("Mir Byte Order: ");
    printf("0x%02X ", mir.byteOrder);
    printNewLine();
#endif

    // Assign the File Path
    memcpy(&mir.fileLocation, &asciiConfigSettings.fileLocation, asciiConfigSettings.sizes[MIR_RECORD_FILE_LOCATION]);

#if    DEBUG_ACTIVE
    printf("file location:");
    printString((char*)mir.fileLocation, asciiConfigSettings.sizes[MIR_RECORD_FILE_LOCATION]);
    printNewLine();
#endif

    // Assign the Output File Name, but only if the mode is set to appender mode
    memcpy(&mir.outputFileName, &asciiConfigSettings.outputFileName, asciiConfigSettings.sizes[MIR_RECORD_FILE_OUTPUT_NAME]);

#if    DEBUG_ACTIVE
    printf("Output filename:");
    printString((char*)mir.outputFileName, asciiConfigSettings.sizes[MIR_RECORD_FILE_OUTPUT_NAME]);
    printNewLine();
#endif

    // Assign the Output File Name, but only if the mode is set to appender mode
    memcpy(&mir.outputLocation, &asciiConfigSettings.outputLocation, asciiConfigSettings.sizes[MIR_RECORD_FILE_OUTPUT_LOCATION]);

#if    DEBUG_ACTIVE
    printf("Output location:");
    printString((char*)mir.outputLocation, asciiConfigSettings.sizes[MIR_RECORD_FILE_OUTPUT_LOCATION]);
    printNewLine();
#endif

    // Assign the Output File Name, but only if the mode is set to appender mode
    memcpy(&mir.fileName, &asciiConfigSettings.fileName, asciiConfigSettings.sizes[MIR_RECORD_FILE_NAME]);

#if    DEBUG_ACTIVE
    printf("filename:");
    printString((char*)mir.fileName, asciiConfigSettings.sizes[MIR_RECORD_FILE_NAME]);
    printNewLine();
#endif

    // Copy the size array
    memcpy(&mir.sizes, asciiConfigSettings.sizes, NUM_OF_MIR_RECORD_TYPES);


    // Configure the file paths
    // Determining if this needs to be done, it might just be the current directory
    if(mir.sizes[MIR_RECORD_FILE_LOCATION] != 0)
    {
#if    DEBUG_ACTIVE
        printf("File location not in current directory\n");
#endif
        // Input path
        mir.sizeOfInputPath = mir.sizes[MIR_RECORD_FILE_LOCATION] + mir.sizes[MIR_RECORD_FILE_NAME] + 2;
        memcpy(mir.fullFileInputPath, mir.fileLocation, mir.sizes[MIR_RECORD_FILE_LOCATION]);
        mir.fullFileInputPath[mir.sizes[MIR_RECORD_FILE_LOCATION]] = '/';
        memcpy(&mir.fullFileInputPath[(mir.sizes[MIR_RECORD_FILE_LOCATION] + 1)], mir.fileName, mir.sizes[MIR_RECORD_FILE_NAME]);
    }
    else
    {
#if    DEBUG_ACTIVE
        printf("File location in current directory\n");
#endif
        memcpy(&mir.fullFileInputPath[mir.sizes[MIR_RECORD_FILE_LOCATION]], mir.fileName, mir.sizes[MIR_RECORD_FILE_NAME]);
    }


#if DEBUG_ACTIVE
    printf("Total File Path: ");
    printString(mir.fullFileInputPath, mir.sizeOfInputPath);
    printNewLine();
#endif

    // Determining if this needs to be done, it might just be the current directory
    if(mir.sizes[MIR_RECORD_FILE_OUTPUT_LOCATION] != 0)
    {
#if    DEBUG_ACTIVE
        printf("File output location not in current directory\n");
#endif
        mir.sizeOfOutputPath = mir.sizes[MIR_RECORD_FILE_OUTPUT_LOCATION] + mir.sizes[MIR_RECORD_FILE_OUTPUT_NAME] + 2;
        memcpy(mir.fullFileOutputPath, mir.outputLocation, mir.sizes[MIR_RECORD_FILE_OUTPUT_LOCATION]);
        mir.fullFileOutputPath[mir.sizes[MIR_RECORD_FILE_OUTPUT_LOCATION] ]= '/';
        memcpy(&mir.fullFileOutputPath[(mir.sizes[MIR_RECORD_FILE_OUTPUT_LOCATION] + 1)], mir.outputFileName, mir.sizes[MIR_RECORD_FILE_OUTPUT_NAME]);
    }
    else
    {
        memcpy(&mir.fullFileOutputPath[mir.sizes[MIR_RECORD_FILE_OUTPUT_LOCATION]], mir.outputFileName, mir.sizes[MIR_RECORD_FILE_OUTPUT_NAME]);
    }

#if DEBUG_ACTIVE
    printf("Total Output File Path: ");
    printString(mir.fullFileOutputPath, mir.sizeOfOutputPath);
    printNewLine();
#endif


    // Assign the File Type, but only if the mode is set to appender mode
    memcpy(&mir.fileType, &asciiConfigSettings.fileType, asciiConfigSettings.sizes[MIR_RECORD_FILE_TYPE]);

#if    DEBUG_ACTIVE
    printf("File I/O Type:");
    printString((char*)mir.fileType, asciiConfigSettings.sizes[MIR_RECORD_FILE_TYPE]);
    printNewLine();
#endif

    return retVal;
}


void retrieveFields(FILE* fp)
{
    // Read contents from file
    char c;
    uint8_t lineBuffer[MAX_STR_LEN] = "";
    uint8_t lineCount = 0u;
    uint8_t lineIndex = 0u;

    c = fgetc(fp);
    while (c != EOF)
    {
        c = fgetc(fp);
        lineBuffer[lineIndex] = c;
        ++lineIndex;

        if(c == '\n')
        {
#if     DEBUG_ACTIVE
            printf("line buffer: ");
            for(int i = 0; i < sizeof(lineBuffer); ++i)
                printf("%c", lineBuffer[i]);
#endif
            lineIndex = 0;
            // Line is over, parse the line and store the stuff
            decipherInfo(lineBuffer, lineCount);
            memset(lineBuffer, 0u, sizeof(lineBuffer));

            // Increment the line count
            ++lineCount;
        }
    }

    fclose(fp);
}

void decipherInfo(uint8_t* info, uint8_t lineCount)
{
    // Get the info and store it locally
    uint8_t infoHold[MAX_STR_LEN] = "";

    switch (lineCount)
    {
    case MIR_RECORD_OPERATING_MODE:
        getRecordInformation(info, (uint8_t*)asciiConfigSettings.operatingMode, lineCount);
        break;
    case MIR_RECORD_CRC_INFO:
        getRecordInformation(info, (uint8_t*)asciiConfigSettings.crcFunctionPtr, lineCount);
        break;
    case MIR_RECORD_BYTE_ORDER:
        getRecordInformation(info, (uint8_t*)asciiConfigSettings.byteOrder, lineCount);
        break;
    case MIR_RECORD_APPENDAGE_ADDRESS:
        getRecordInformation(info, (uint8_t*)asciiConfigSettings.appendAddress, lineCount);
        break;
    case MIR_RECORD_FILE_LOCATION:
        getRecordInformation(info, (uint8_t*)asciiConfigSettings.fileLocation, lineCount);
        break;
    case MIR_RECORD_FILE_NAME:
        getRecordInformation(info, (uint8_t*)asciiConfigSettings.fileName, lineCount);
        break;
    case MIR_RECORD_FILE_OUTPUT_NAME:
        getRecordInformation(info, (uint8_t*)asciiConfigSettings.outputFileName, lineCount);
        break;
    case MIR_RECORD_FILE_OUTPUT_LOCATION:
        getRecordInformation(info, (uint8_t*)asciiConfigSettings.outputLocation, lineCount);
        break;
    case MIR_RECORD_FILE_TYPE:
        getRecordInformation(info, (uint8_t*)asciiConfigSettings.fileType, lineCount);
        break;
    default:
        break;
    }
}

void getRecordInformation(uint8_t* info, uint8_t* infoToBeStored, uint8_t lineIndex)
{
    bool infoFlag = false;
    uint8_t infoIndex = 0u;

    for(int i = 0; i < MAX_STR_LEN; ++i)
    {
        if (info[i] == ';')
        {
            // make note of the size for when we re-write the file
            asciiConfigSettings.sizes[lineIndex] = infoIndex;
            break; // Will break the loop once the line is over
        }

        if (infoFlag == true)
        {
            infoToBeStored[infoIndex] = info[i];
            ++infoIndex;
        }

        // Check for the beginning of a line
        if (info[i] == ':')
        {
            infoFlag = true;
        }
    }
    
#if    DEBUG_ACTIVE
    printf("\ninfo to be stored: ");
    for(int j = 0; j < asciiConfigSettings.sizes[lineIndex]; ++j)
    {
        printf("%c", infoToBeStored[j]);
    }
    printNewLine();
    #endif
}
