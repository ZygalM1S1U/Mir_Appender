#include "help.h"
#include "main.h"
#include "utils.h"

void mainHelp(void)
{
    bool help = true;
    char menuVar = 0;

    printf("\nWelcome to the help menu for Mir\n");
    printf("Options: \n");
    printf("\n1. About\n2. Features\n3. Quick Build Mode\n4. Current Configuration Settings\n5. ...\n6. Exit\n\n");
    printf("USER CURSOR: ");

    while(help)
    {
        scanf("%s", &menuVar);
        menuVar-=48;
        switch(menuVar)
        {
        case 1:
            help = true;
            aboutMenu();
            break;
        case 2:
            help = true;
            featureMenu();
            break;
        case 3:
            help = true;
            qBMenu();
            break;
        case 4:
            help = true;
            ConfigMenu();
            break;
        case 5:
            help = true;
            special();
            break;
        case 6:
            help = false;
            break;
        default:
            help = true;
            special();
            break;
        }
        printf("Help Menu\n");
        printf("\n1. About\n2. Features\n3. Quick Build Mode\n4. Current Configuration Settings\n5. ...\n6. Exit\n\n");
        printf("USER CURSOR: ");
    }
}

void aboutMenu(void)
{
    printf("Mir is an Intel Hex parser and CRC appender.  It was written by James Novorita\n");
    printf("This was intended to make the embedded build process easier with a bootloader.\n");
    printf("It allows for a CRC to be appended to an Intel Hex File\n");
    printf("Current Revision: %d.%d.%d.%d\n", MIR_MAJOR, MIR_MINOR, MIR_BUILD, MIR_DEV);
    printf("Second major release\n\
           Application features:\n\
           The application has three modes of operation: Appender mode, Test Mode, and UI mode\n\
           Appender Mode: Based on the file path, it will parse an intel hex file, calculate a CRC, and append both the length and crc to the file.\n\
           Test Mode: Allows testing of newly added CRC functions (Placeholde rin for development)\n\
           UI Mode: Will walk you through each piece of the program and allow you to see the inner workings\n\
           - The configuration settings are starting to be put in place.  ImGui will be added for reading config files.\n\
           - CRC functions can be added by following the function pointer protocol specified in config.c  Make sure to update the ENUM and the function pointers are updated correctly.\n\
           - Basic funtionality is implemented.  Correctly calculates a CRC for a hex file.\n\
           - Appender Mode is now active.\n\
           - Test mode may be implemented for custom tool running\n\
           UI mode provides context and debug information for understanding the tool's workings\n");
}

void featureMenu(void)
{
    printf("Mir has two main program modes.\nThe interface mode allows you to step through the process and see how things work in slow motion.\n");
    printf("Build mode allows to append the executable to a post build script and pass an address through an argument, or in the configuration file.\n");
    printf("CRC Variants supported are:\n");
    printf("1. CRC-16 CCIT\n");
    printf("2. CRC-16 XMOD (Placeholder, not implemented)\n");
    printf("3. CRC-16 IEC60335\n");

}

void qBMenu(void)
{
    printf("This mode will allow for appendage to occur without the UI or debug information.\n");
    printNewLine();
}

void ConfigMenu(void)
{
    printf("Current Configuration Settings");
    printNewLine();
    printf("Append Address: ");
    printf("0x%08X ", mir.appendageAddressFile);
    printNewLine();

    printf("CRC: ");
    printf("0x%02X ", mir.crcSetting.crcFuncPtrIndex);
    printNewLine();

    printf("Mir Mode: ");
    printf("0x%02X ", mir.mirMode);
    printNewLine();

    printf("Mir Byte Order: ");
    printf("0x%02X ", mir.byteOrder);
    printNewLine();

    printf("file location:");
    printString((char*)mir.fileLocation, sizeof(mir.fileLocation));
    printNewLine();


    printf("Output filename:");
    printString((char*)mir.outputFileName, sizeof(mir.outputFileName));
    printNewLine();


    printf("Output location:");
    printString((char*)mir.outputLocation, sizeof(mir.outputLocation));
    printNewLine();


    printf("filename:");
    printString((char*)mir.fileName, sizeof(mir.fileName));
    printNewLine();

    printf("Total File Path: ");
    printString(mir.fullFileInputPath, mir.sizeOfInputPath);
    printNewLine();

    printf("Total Output File Path: ");
    printString(mir.fullFileOutputPath, mir.sizeOfOutputPath);
    printNewLine();



}

void special(void)
{
    printf(":)\n");

}

