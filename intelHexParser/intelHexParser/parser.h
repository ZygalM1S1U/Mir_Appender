#ifndef _PARSER_H__
#define _PARSER_H__

#include "main.h"

#define END_OF_LINE_DELIMITER       "\n"
#define BEGINNING_OF_LINE_DELIMITER ":"
#define RECORD_SIZE_DELIMITER       0x01
#define INFORMATION_DELIMITER       6
#define HEX_RECORD_SIZE             1
#define HEX_RECORD_ADDRESS          3
#define HEX_RECORD_TYPE             6

typedef enum PARSE_STATES_t
{
    FILE_IO,
    FILE_EXTRACT,
    FILE_PARSE,
    DATA_STORE,
    CALC_CRC,
    APPEND_CRC,
    HALT,
    NUM_OF_STATES
}PARSE_STATES;

/// @brief Parse the record
void parse(char* hexFilePath);

/// @brief Extract the record pushed from the fileio module
PARSE_STATES extractRecord(void);

/// @brief Analyze the record, split it into the known intel hex format
void analyzeRecord(uint8_t* recordBuffer, uint8_t recordSize);

/// @brief A record store for the global array
void recordStore(uint8_t tempSlidePass, uint8_t* sizeTypeChksmhexBuffer, uint8_t* addressHexBuffer);

#endif // _PARSER_H__
