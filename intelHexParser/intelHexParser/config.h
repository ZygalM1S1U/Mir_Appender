#ifndef _CONFIG_H__
#define _CONFIG_H__

#include "main.h"

#define MIR_CONFIG_RECORD_MAX_SIZE      30
#define MAX_STR_LEN                     256
#define INDEX_UI                        3
#define INDEX_CRC                       4
#define INDEX_BYTE                      5
#define INDEX_APPEND                    6
#define INDEX_FILE_LOC                  8
#define INDEX_FILE_NAME                 10
#define INDEX_OUTPUT_NAME               7
#define INDEX_OUTPUT_LOC                6

typedef enum MIR_CONFIG_FILE_RECORD_TYPES_t
{
    MIR_RECORD_OPERATING_MODE,
    MIR_RECORD_CRC_INFO,
    MIR_RECORD_BYTE_ORDER,
    MIR_RECORD_APPENDAGE_ADDRESS,
    MIR_RECORD_FILE_LOCATION,
    MIR_RECORD_FILE_NAME,
    MIR_RECORD_FILE_OUTPUT_NAME,
    MIR_RECORD_FILE_OUTPUT_LOCATION,
    MIR_RECORD_FILE_TYPE,
    NUM_OF_MIR_RECORD_TYPES
}MIR_CONFIG_FILE_RECORD_TYPES;

typedef enum MIR_OPERATING_MODES_t
{
    MIR_APPENDER_MODE,
    MIR_UI_MODE,
    MIR_TEST_MODE,
    MIR_NUM_MODES
}MIR_OPERATING_MODES;

typedef enum MIR_BYTE_ORDER_CONFIG_t
{
    MIR_LITTLE_ENDIAN,
    MIR_BIG_ENDIAN,
    MIR_MIDDLE_ENDIAN,
    MIR_PDP_ENDIAN,
    MIR_HONEYWELL316_ENDIAN,
    MIR_BYTE_SWAPPED,
    MIR_NUM_OF_BYTE_ORDER
}MIR_BYTE_ORDER_CONFIG;

typedef enum CRC_FUNCTIONS_t
{
    CRC16CCIT = 0u,
    CRC16_X_MOD,
    CRC16_IEC_60335,
    NUM_OF_CRC_FUNCS
}CRC_FUNCTIONS;

typedef enum MIR_FILE_TYPES_t
{
    INTEL_HEX = 0u,
    SREC,
    NUM_OF_FILE_TYPES
}MIR_FILE_TYPES;

typedef uint16_t (*CrcFunction)(const uint8_t* data, const long size);


typedef struct
{
    // Crc function
    /// @note this function's input constraints
    CrcFunction crcCalculateFunc;
    uint16_t crcFuncPtrIndex;
    uint8_t  inputDataInversionMode;
    uint8_t  outputDataInversionMode;
    uint32_t seedValue;
    uint8_t  crcLengthType;
    uint32_t generatingPolynomial;
    uint8_t  inputDataFormatBytes;
}crcSettings;

typedef struct
{
    FILE*   		      mirConfigurationFile;
    crcSettings		      crcSetting;
    uint32_t 		      appendageAddressFile;
    fpos_t  		      currentLocation;
    MIR_OPERATING_MODES   mirMode;
    MIR_BYTE_ORDER_CONFIG byteOrder;
    MIR_FILE_TYPES        fileType;
    char fileLocation[MAX_STR_LEN];
    char fileName[MAX_STR_LEN];
    char outputLocation[MAX_STR_LEN];
    char outputFileName[MAX_STR_LEN];
    char fullFileInputPath[MAX_STR_LEN];
    uint8_t sizeOfInputPath;
    char fullFileOutputPath[MAX_STR_LEN];
    uint8_t sizeOfOutputPath;
    uint8_t sizes[NUM_OF_MIR_RECORD_TYPES];

    // Stats
    double lastKnownExecutionTime;
}mirConfiguratonSettings;

mirConfiguratonSettings mir;

/// @brief Pulls the configuration data from the config files
bool retrieveConfigurationSettings(void);


#endif // _CONFIG_H__
