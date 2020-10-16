#include "main.h"

bool programRunning = true;
bool mainMenu = true;
PROGRAM_STATES state = MAIN_MENU_STATE;
MENU_STATES menuState = MAIN_MENU_USER_PROMPT;
errorArg errorStateVar = OK;
menuItem menuPrompt = 1;
uint16_t testResult = 0u;
uint8_t crcTest[5] = {0x01, 0x03, 0x55, 0x67, 0x88};

void crcTestFunc(void);

/// @section Private function prototypes
void killProgram(void);

PROGRAM_STATES mainMenuHandle(void);

PROGRAM_STATES currentMenuPrint(MENU_STATES currentMenuState, char* inputString);

int main(void);

void uiUpdate(void);

/// @section modes
/// @brief UI mode will have the prompt appear on the command line
void uiMode(void);

/// @brief appender mode will use the settings configured to use post build mode
void appenderMode(void);

/// @brief a template for testing the protocol and new CRC functions that have been added
void testMode(void);

PROGRAM_STATES mainMenuState(void);

/// @brief Build the menu
void menuBuilder(menuItem menu);

/// @brief The input handler will deal with console input from any entry point
/// @return error codes returned for for the menu states
char inputHandler(MENU_STATES state, char* param);

bool loadingScreen(void);

/// @brief Error state will handle how to display errors detected inside the program
void errorState(errorArg error);

/// @brief Print the current time when the program starts
void printTime(void);

int main(void)
{
    // If this does not pull the correct config settings, then it will crash the program
    bool result = retrieveConfigurationSettings();

    if(result)
    {
        switch(mir.mirMode)
        {
        case MIR_UI_MODE:
            uiMode();
            break;
        case MIR_APPENDER_MODE:
            appenderMode();
            break;
        case MIR_TEST_MODE:
            testMode();
            break;
        default:
            // Mode not defined
            printf("Mode not defined\n");
            break;
        }
    }
    else
    {
        // Fatal Error
        printf("Fatal Error: Configuration file has not been parsed correctly");
    }

    return 0;
}

void uiMode(void)
{
    printNewLine();
    printf("Welcome to Mir, the Intel hex parser and CRC appender");
    printTime();
    printf("Revision: %d.%d.%d.%d\n", MIR_MAJOR, MIR_MINOR, MIR_BUILD, MIR_DEV);
    printArt();

    // charge up the loading screen
    bool loadingResult = loadingScreen();

    if(loadingResult)
    {
        // Start the parser before the CRC
        while(programRunning)
        {
            uiUpdate();
        }
    }
    else
    {
        errorState(errorStateVar);
    }
    printf("Goodbye!\n");
    getchar();

}

void appenderMode(void)
{
    // Just do the thing and no one gets hurt.
    /// @todo Pull the file name, file path, etc... from the config file
    // Start the parsing
    parse(mir.fullFileInputPath);
}

void testMode(void)
{
    /// @todo Build your own test mode
}

void uiUpdate(void)
{
    switch(state)
    {
    case MAIN_MENU_STATE:
        // main menu
        state = mainMenuState();
        break;
    case PARSE_STATE:
        // Start the parsing
        state = mainMenuState();
        break;
    case CRC_STATE:
        state = mainMenuState();
        break;
    case HELP_STATE:
        state = mainMenuState();
        break;
    case KILL_STATE:
        killProgram();
        break;
    default:
        printf("erroneous state detected\n");
        break;
    }

    /// @todo insert error handler

}

PROGRAM_STATES mainMenuState(void)
{
    char inputString[MAX_STR_LEN];
    PROGRAM_STATES retVal = MAIN_MENU_STATE;

    if(menuState == MAIN_MENU_USER_PROMPT)
    {
        menuBuilder(menuPrompt);
    }

    // Main menu handling
    retVal = currentMenuPrint(menuState, inputString);
    memset(inputString, 0, sizeof(inputString));

    return retVal;
}

PROGRAM_STATES currentMenuPrint(MENU_STATES currentMenuState, char* inputString)
{
    PROGRAM_STATES retVal;

    switch(currentMenuState)
    {
    case MAIN_MENU_USER_PROMPT:
        retVal = mainMenuHandle();
        break;
    case PARSE_HEX_MENU:
        // Start the parsing process
        printf("Please specify the name of the file to parse.\nMake sure the working directory has the file.\n");
        menuPrompt = inputHandler(menuState, inputString);
        // Check out the input string
        if(strlen(inputString))
        {
            // If the length is less than 0, proceed.
            // Start the parsing task
            parse(inputString);
            menuState = MAIN_MENU_USER_PROMPT;
            menuPrompt = 1;
        }
        break;
    case CRC_TEST_STATE:
        crcTestFunc();
        menuState = MAIN_MENU_USER_PROMPT;
        retVal = 1;
        menuPrompt = 1;
        break;
    case HELP_STATE:
        mainHelp();
        menuState = MAIN_MENU_USER_PROMPT;
        retVal = 1;
        menuPrompt = 1;
        break;
    default:
        printf("\n");
        menuPrompt = 1;
        retVal = 1;
        menuState = MAIN_MENU_USER_PROMPT;
        break;
    }

    return retVal;
}

void menuBuilder(menuItem menu)
{
    printNewLine();

    // Print the menu
    printf(MAIN_MENU);

    switch(menu)
    {
    case 1:
        // Print the main menu screen
        printf(MENU_ITEM_1);
        printf(MENU_ITEM_2);
        printf(MENU_ITEM_3);
        printf(MENU_ITEM_4);
        break;
    default:
        printf("Incorrect commannd, try again.\n");
        break;
    }

    printNewLine();
}

char inputHandler(MENU_STATES state, char* param)
{
    char retVal = 0;
    char localString[MAX_STR_LEN] = "";
    printf("USER CURSOR: ");

    switch(state)
    {
    case MAIN_MENU_STATE:
        scanf("%s", &localString);
        retVal = localString[0] - 48;
        break;
    case PARSE_HEX_MENU:
        // We know there is going to need to be a string for the file
        scanf("%s", &localString);
        memcpy(param, localString, sizeof(localString));
        break;
    case CRC_TEST_STATE:
        scanf("%s", &localString);
        memcpy(param, localString, sizeof(localString));
        break;
    case HELP:
        // Read the input from the help
        scanf("%s", &localString);
        memcpy(param, localString, sizeof(localString));
        retVal = (uint8_t)param[0];
        break;
    case KILL:
        scanf("%s", &localString);
        memcpy(param, localString, sizeof(localString));
        break;
    default:
        // Empty will indicate a bad state
        printf("Not a valid entry-point...\n\n");
        break;
    }
    return retVal;
}

PROGRAM_STATES mainMenuHandle(void)
{
    PROGRAM_STATES retVal = MAIN_MENU_STATE;

    /// @note default state is the state that lists options
    menuPrompt = inputHandler(menuState, NULL);

    // Copy the menu prompt
    menuState = menuPrompt;

    switch(menuPrompt)
    {
    case PARSE_HEX_MENU:
        retVal = PARSE_STATE;
        break;
    case CRC_TEST_STATE:
        retVal = CRC_STATE;
        break;
    case HELP:
        retVal = HELP_STATE;
        break;
    case KILL:
        retVal = KILL_STATE;
        break;
    default:
        printf("Invalid input\n");
        retVal = 1;
        break;
    }

    return retVal;
}

bool loadingScreen(void)
{
    bool retVal = false;

    // If no errors are returned, continue
    if(memCheck())
    {
        retVal = true;
    }
    else
    {
        printf("Heap Error\n");
    }

    return retVal;
}

void errorState(errorArg error)
{
    ///@todo make error table
    printf("\nError, return status %d", error);
}

void printTime(void)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    // Print local date and time
    printf("\nLocal Date & Time: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900,
           tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
}

void killProgram(void)
{
    programRunning = false;
}

void crcTestFunc(void)
{
    /*printf("CRC Test for the CCIT-16-bit variant is commencing\n");
    printf("Testing data calcuation for array:\n");
    for(int i = 0; i < sizeof(crcTest); ++i)
        printf("0x%02X ", crcTest[i]);
    printf("\n");
    testResult = crcCCITT16Calculate(crcTest, sizeof(crcTest));
    printf("test result: 0x%04X\n", testResult);
    uint8_t frameBuffer[8];
    memset(frameBuffer, 0u, sizeof(frameBuffer));
    frameBuffer[0] = 0x04;
    frameBuffer[1] = 0x97;
    frameBuffer[2] = 0x90;
    printf("Checksum 256 Test: 0x%02X\n", checksum2sComp(frameBuffer, 8));*/
    printf("Implement your own test in here.\n");
}

