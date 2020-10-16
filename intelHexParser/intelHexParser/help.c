#include "help.h"
#include "main.h"
#include "utils.h"

void mainHelp(void)
{
    bool help = true;
    int menuVar = 0;

    printf("\nWelcome to the help menu for Mir\n");
    printf("Options: \n");
    printf("\n1. About\n2. Features\n3. Quick Build Mode\n4. Current Configuration Settings\n5. ...\n6. Exit\n\n");
    printf("USER CURSOR: ");

    while(help)
    {
        scanf("%u", &menuVar);

        switch(menuVar)
        {
        case 1:
            aboutMenu();
            break;
        case 2:
            featureMenu();
            break;
        case 3:
            qBMenu();
            break;
        case 4:
            ConfigMenu();
            break;
        case 5:
            special();
            break;
        case 6:
            help = false;
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

