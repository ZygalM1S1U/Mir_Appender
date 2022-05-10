#ifndef _MAIN_H__
#define _MAIN_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "parser.h"
#include "fileio.h"
#include "revision.h"
#include "utils.h"
#include "help.h"
#include "config.h"

/// @note Debug param
#define DEBUG_ACTIVE    1

/// @section String list
#define MAIN_MENU       "Main Menu\n"
#define MENU_ITEM_1     "1. Parse Hex File\n"
#define MENU_ITEM_2     "2. Custom Test Template\n"
#define MENU_ITEM_3     "3. Help\n"
#define MENU_ITEM_4     "4. Exit\n"


typedef uint8_t menuItem;
typedef uint8_t errorArg;

typedef struct
{
    //File in hex with size
    unsigned long fileSize;
    uint8_t* hexFileBuffer;
}hexFile;

hexFile intelHexFile;

typedef enum PROGRAM_STATES_t
{
    MAIN_MENU_STATE,
    PARSE_STATE,
    CRC_STATE,
    HELP_STATE,
    KILL_STATE,
    CONFIG_STATE,
    NUM_STATES
}PROGRAM_STATES;

typedef enum ERRORS_t
{
    OK,
    DYNAMIC_MEMORY_ERROR,
    NUM_ERRORS
}ERRORS;

typedef enum MENU_STATES_t
{
    MAIN_MENU_USER_PROMPT,
    PARSE_HEX_MENU,
    CRC_TEST_STATE,
    HELP,
    KILL,
    NUM_MENU_STATES
}MENU_STATES;

typedef enum CONFIG_OPTIONS_t
{
    MODE_OF_OPERATION,
    CRC_TYPE,
    CRC_POLY,
    CRC_SIZE,
    CRC_VARIANT,
    NUM_OF_CONFIG_OPTIONS
}CONFIG_OPTIONS;

typedef enum MODES_t
{
    INTERACTIVE,
    BUILD,
    NUM_MODES
}MODES;

typedef enum CRC_TYPES_t
{
    CRC_CCIT,
    NUM_CRC
}CRC_TYPES;

typedef enum CRC_POLYNOMIAL_t
{
    OXFFFF,
    NUM_OF_POLY
}CRC_POLYNOMIAL;

typedef enum CRC_SIZE_BITS_t
{
    EIGHT_BIT,
    SIXTEEN_BIT,
    THIRTY_TWO_BIT,
    SIXTY_FOUR_BIT
}CRC_SIZE_BITS;

#endif // _MAIN_H__
