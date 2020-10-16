#include "menu_config_component.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "application.h"
#include <stdio.h>
#include "addons/imguidock.h"

#ifdef WIN32
#include "dirent.h"
#else
#include <dirent.h>
#endif

#define MIR_CONFIG_RECORD_MAX_SIZE      30      
#define MAX_STR_LEN						255
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
    NUM_OF_MIR_RECORD_TYPES,
    MIR_RECORD_UNDEFINED = 255
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

struct asciiConfigSettings_t
{
    char operatingMode[2];            /// @note this needs to changed if there are more than 9 op modes
    char appendAddress[16];			  /// @note This needs to be changed for anything larger than 64 bit addressing
    char crcFunctionPtr[2];           /// @note this needs to changed if there are more than 9 function pointers
    char byteOrder[2];
    char fileLocation[MAX_STR_LEN];
    char fileName[MAX_STR_LEN];       /// @note this must be changed if the string is longer than 24
    char outputLocation[MAX_STR_LEN];
    char ouputFileName[MAX_STR_LEN];
    uint8_t sizes[NUM_OF_MIR_RECORD_TYPES];     // The size of each record entry to save for later
    long indicies[NUM_OF_MIR_RECORD_TYPES]; // The index of each record entry
}asciiConfigSettings;

uint8_t operatingMode = 0u;            /// @note this needs to changed if there are more than 9 op modes
uint8_t crcFunctionPtr = 0u;           /// @note this needs to changed if there are more than 9 function pointers
uint8_t byteOrder = 0u;

static bool configFlags[NUM_OF_MIR_RECORD_TYPES] = {false, false, false, false, false, false, false, false};

const char* crcCalcFunc[] =
{
    "CRC16CCIT",
    "CRC16_X_MOD",
    "CRC16_IEC_60335"
};

const char* items[] =
{
    "MIR_LITTLE_ENDIAN",
    "MIR_BIG_ENDIAN",
    "MIR_MIDDLE_ENDIAN",
    "MIR_PDP_ENDIAN",
    "MIR_HONEYWELL316_ENDIAN",
    "MIR_BYTE_SWAPPED"
};

const char* itemsMode[] =
{
    "MIR_APPENDER_MODE",
    "MIR_UI_MODE",
    "MIR_TEST_MODE"
};

MenuConfigComponent::MenuConfigComponent()
    : height(0),
      loggingOn(false),
      first(false)
{
    std::memset(inputBuffer, 0u, sizeof(inputBuffer));
    std::memset(inputFileNameBuffer, 0u, sizeof(inputFileNameBuffer));
    std::memset(inputFileLocBuffer, 0u, sizeof(inputFileLocBuffer));
    std::memset(inputFileOpNameBuffer, 0u, sizeof(inputFileOpNameBuffer));
    std::memset(inputFileOpLocBuffer, 0u, sizeof(inputFileOpLocBuffer));
    pullAllFileAttributes();
}

MenuConfigComponent::~MenuConfigComponent()
{
}

float MenuConfigComponent::getHeight()
{
    return height;
}

void MenuConfigComponent::draw(float elapsedTime)
{
    if (ImGui::BeginDock("Mir Settings"))
    {
        // everything under this is scrollable
        ImGui::BeginChild("Scrolling");


        if (first)
            ImGui::SetNextTreeNodeOpen(false);

        if (ImGui::TreeNode("Appendage Address"))
        {
            ImGui::Text("0x");
            ImGui::SameLine();
            // Text input will be 10% of the window width
            ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.1f);

            // slider will always be 165 pixels wide
            ImGui::PushItemWidth(165);

            ImGui::InputText("", (char*)inputBuffer, sizeof(inputBuffer));
            ImGui::SameLine();
            char address[15] = "";
            address[0] = '0';
            address[1] = 'x';
            ImGui::Text("");
            ImGui::SameLine();
            ImGui::Text(std::strcat(address, asciiConfigSettings.appendAddress));
            ImGui::TreePop();
            ImGui::SameLine();
            ImGui::Checkbox("  ", &configFlags[MIR_RECORD_APPENDAGE_ADDRESS]);
        }

        if (first)
            ImGui::SetNextTreeNodeOpen(false);

        if (ImGui::TreeNode("Byte Order Selection"))
        {
            // Text input will be 10% of the window width
            ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.1f);

            // slider will always be 165 pixels wide
            ImGui::PushItemWidth(165);
            static const char* selectedByteOrder = "MIR_LITTLE_ENDIAN";
            static ImGuiComboFlags byteFlags = 0;

            if (ImGui::BeginCombo(" ", selectedByteOrder, byteFlags))
            {
                for (int i = 0; i < IM_ARRAYSIZE(items); ++i)
                {
                    bool isBSelected = (selectedByteOrder == items[i]);
                    if (ImGui::Selectable(items[i], isBSelected))
                    {
                        selectedByteOrder = items[i];
                    }
                    if (isBSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            ImGui::SameLine();
            ImGui::Text("  Byte Order selected: ");
            ImGui::SameLine();
            if (byteOrder >= MIR_NUM_OF_BYTE_ORDER)
            {
                ImGui::Text("INVALID PARAMETER OF BYTE ORDER: %i", byteOrder);
            }
            else
            {
                ImGui::Text(items[byteOrder]);
            }
            ImGui::SameLine();
            ImGui::Checkbox("          ", &configFlags[MIR_RECORD_BYTE_ORDER]);
            ImGui::TreePop();
            ImGui::Separator();
        }

        if (first)
            ImGui::SetNextTreeNodeOpen(false);

        if (ImGui::TreeNode("Mir Mode Selection"))
        {
            // Text input will be 10% of the window width
            ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.1f);

            // slider will always be 165 pixels wide
            ImGui::PushItemWidth(165);
            static const char* selectedMode = "MIR_APPENDER_MODE";
            static ImGuiComboFlags modeFlags = 0;

            if (ImGui::BeginCombo(" ", selectedMode, modeFlags))
            {
                for (int j = 0; j < IM_ARRAYSIZE(itemsMode); ++j)
                {
                    bool isSelected = (selectedMode == itemsMode[j]);
                    if (ImGui::Selectable(itemsMode[j], isSelected))
                    {
                        selectedMode = itemsMode[j];
                    }
                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::SameLine();
            ImGui::Text("  Mode selected: ");
            ImGui::SameLine();
            if (operatingMode >= MIR_NUM_MODES)
            {
                ImGui::Text("INVALID MODE IN CONFIG FILE OF TYPE: %i", operatingMode);
            }
            else
            {
                ImGui::Text(itemsMode[operatingMode]);
            }
            ImGui::SameLine();
            ImGui::Checkbox("   ", &configFlags[MIR_RECORD_OPERATING_MODE]);
            ImGui::TreePop();
            ImGui::Separator();
        }

        if (first)
            ImGui::SetNextTreeNodeOpen(false);

        if (ImGui::TreeNode("CRC Funtion Selection"))
        {
            // Text input will be 10% of the window width
            ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.1f);

            // slider will always be 165 pixels wide
            ImGui::PushItemWidth(165);
            static const char* selectedCRC = "CRC16CCIT";
            static ImGuiComboFlags crcFlags = 0;

            if (ImGui::BeginCombo(" ", selectedCRC, crcFlags))
            {
                for (int j = 0; j < IM_ARRAYSIZE(crcCalcFunc); ++j)
                {
                    bool isCSelected = (selectedCRC == crcCalcFunc[j]);
                    if (ImGui::Selectable(crcCalcFunc[j], isCSelected))
                    {
                        selectedCRC = crcCalcFunc[j];
                    }
                    if (isCSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            ImGui::SameLine();
            ImGui::Text("  CRC Function selected: ");
            ImGui::SameLine();
            if (crcFunctionPtr >= NUM_OF_CRC_FUNCS)
            {
                ImGui::Text("INVALID CRC FUNCTION PARAMETER IN CONFIG FILE: %i", crcFunctionPtr);
            }
            else
            {
                ImGui::Text(crcCalcFunc[crcFunctionPtr]);
            }
            ImGui::SameLine();
            ImGui::Checkbox("        ", &configFlags[MIR_RECORD_CRC_INFO]);
            ImGui::TreePop();
            ImGui::Separator();
        }

        if (first)
            ImGui::SetNextTreeNodeOpen(false);

        if (ImGui::TreeNode("File Location"))
        {
            // Text input will be 10% of the window width
            ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.1f);

            // slider will always be 165 pixels wide
            ImGui::PushItemWidth(165);
            ImGui::InputText("    File Location selected:", (char*)inputFileLocBuffer, sizeof(inputFileLocBuffer));
            ImGui::SameLine();
            ImGui::Text("");
            ImGui::SameLine();
            ImGui::Text(asciiConfigSettings.fileLocation);
            ImGui::TreePop();
            ImGui::SameLine();
            ImGui::Checkbox("    ", &configFlags[MIR_RECORD_FILE_LOCATION]);
            ImGui::Separator();
        }

        if (first)
            ImGui::SetNextTreeNodeOpen(false);

        if (ImGui::TreeNode("File Name"))
        {
            // Text input will be 10% of the window width
            ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.1f);

            // slider will always be 165 pixels wide
            ImGui::PushItemWidth(165);
            ImGui::InputText("    File Name selected:", (char*)inputFileNameBuffer, sizeof(inputFileNameBuffer));
            ImGui::SameLine();
            ImGui::Text("");
            ImGui::SameLine();
            ImGui::Text(asciiConfigSettings.fileName);
            ImGui::TreePop();
            ImGui::SameLine();
            ImGui::Checkbox("      ", &configFlags[MIR_RECORD_FILE_NAME]);
            ImGui::Separator();
        }

        if (first)
            ImGui::SetNextTreeNodeOpen(false);

        if (ImGui::TreeNode("Output File Location"))
        {
            // Text input will be 10% of the window width
            ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.1f);

            // slider will always be 165 pixels wide
            ImGui::PushItemWidth(165);

            ImGui::InputText("    Output File Location selected:", (char*)inputFileOpLocBuffer, sizeof(inputFileOpLocBuffer));
            ImGui::SameLine();
            ImGui::Text("");
            ImGui::SameLine();
            ImGui::Text(asciiConfigSettings.outputLocation);
            ImGui::TreePop();
            ImGui::SameLine();
            ImGui::Checkbox("       ", &configFlags[MIR_RECORD_FILE_OUTPUT_LOCATION]);
            ImGui::Separator();
        }

        if (first)
            ImGui::SetNextTreeNodeOpen(false);

        if (ImGui::TreeNode("Output File Name"))
        {
            {
                // Text input will be 10% of the window width
                ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.1f);

                // slider will always be 165 pixels wide
                ImGui::PushItemWidth(165);

                ImGui::InputText("    Output File Name selected:", (char*)inputFileOpNameBuffer, sizeof(inputFileOpNameBuffer));
                ImGui::SameLine();
                ImGui::Text("");
                ImGui::SameLine();
                ImGui::Text(asciiConfigSettings.ouputFileName);

                ImGui::TreePop();
                ImGui::SameLine();
                ImGui::Checkbox("        ", &configFlags[MIR_RECORD_FILE_OUTPUT_NAME]);
                ImGui::Separator();
            }
        }

        if (ImGui::TreeNode("Set New Config Settings"))
        {
            if (ImGui::Button("Set Settings", ImVec2(100, 30)))
            {
                // Button Event Handling
                // Set config settings
                editAllFileAttributes();
            }

            ImGui::TreePop();
            ImGui::Separator();
        }

        //end scrolling
        ImGui::EndChild();
    }
    ImGui::EndDock();

    first = false;
    pullAllFileAttributes();

}


void MenuConfigComponent::pullAllFileAttributes(void)
{
    FILE* fptr;

    // Open the config file
    // Set the current directory of the file
    /// @note this should be noted, as this will break the program
    fptr = fopen("../../../../intelHexParser/scripts/mirConfig.mir", "r");
    if (fptr == NULL)
    {
        printf("Cannot open config file \n");
        exit(0);
    }
    else
    {
        printf("config file opened\n");
        getAllFields(fptr);
        retrieveConfigurationSettings();
    }

    fclose(fptr);

}

void MenuConfigComponent::editAllFileAttributes(void)
{
    FILE* fp;
    FILE* target;
    char c;
    bool configIndexFound = false;
    MIR_CONFIG_FILE_RECORD_TYPES record = MIR_RECORD_UNDEFINED;

    // Open the config file
    // Set the current directory of the file
    /// @note this should be noted, as this will break the program
    fp = fopen("../../../../intelHexParser/scripts/mirConfig.mir", "r");
    target = fopen("../../../../intelHexParser/scripts/mirConfig_temp.mir", "w");
    if (fp == NULL || target == NULL)
    {
        printf("Cannot open config file \n");
        exit(0);
    }
    else
    {
        while ((c = fgetc(fp)) != EOF)
        {
            
            long ftellIndex = ftell(target);
            if(ftellIndex == asciiConfigSettings.indicies[MIR_RECORD_OPERATING_MODE])
            {
                if (configFlags[MIR_RECORD_OPERATING_MODE])
                {
                    configIndexFound = true;
                    record = MIR_RECORD_OPERATING_MODE;
                }
            }
            else if(ftellIndex == asciiConfigSettings.indicies[MIR_RECORD_CRC_INFO])
            {
                if (configFlags[MIR_RECORD_CRC_INFO])
                {
                    configIndexFound = true;
                    record = MIR_RECORD_CRC_INFO;
                }
            }
            else if(ftellIndex == asciiConfigSettings.indicies[MIR_RECORD_BYTE_ORDER])
            {
                if (configFlags[MIR_RECORD_BYTE_ORDER])
                {
                    configIndexFound = true;
                    record = MIR_RECORD_BYTE_ORDER;
                }
            }
            else if(ftellIndex == asciiConfigSettings.indicies[MIR_RECORD_APPENDAGE_ADDRESS])
            {
                if (configFlags[MIR_RECORD_APPENDAGE_ADDRESS])
                {
                    configIndexFound = true;
                    record = MIR_RECORD_APPENDAGE_ADDRESS;
                }
            }
            else if(ftellIndex == asciiConfigSettings.indicies[MIR_RECORD_FILE_LOCATION])
            {
                if (configFlags[MIR_RECORD_FILE_LOCATION])
                {
                    configIndexFound = true;
                    record = MIR_RECORD_FILE_LOCATION;
                }
            }
            else if (ftellIndex == asciiConfigSettings.indicies[MIR_RECORD_FILE_NAME])
            {
                if (configFlags[MIR_RECORD_FILE_NAME])
                {
                    configIndexFound = true;
                    record = MIR_RECORD_FILE_NAME;
                }
            }
            else if (ftellIndex == asciiConfigSettings.indicies[MIR_RECORD_FILE_OUTPUT_NAME])
            {
                if (configFlags[MIR_RECORD_FILE_OUTPUT_NAME])
                {
                    configIndexFound = true;
                    record = MIR_RECORD_FILE_OUTPUT_NAME;
                }
            }
            else if (ftellIndex == asciiConfigSettings.indicies[MIR_RECORD_FILE_OUTPUT_LOCATION])
            {
                if (configFlags[MIR_RECORD_FILE_OUTPUT_LOCATION])
                {
                    configIndexFound = true;
                    record = MIR_RECORD_FILE_OUTPUT_LOCATION;
                }
            }

            if (configIndexFound)
            {
                // flip the flag back
                configIndexFound = false;

                // Next, append the new stuff
               // appendFieldappendField(fp, record);
            }
            else
            {
                fputc(c, target);
            }
        }
        /*
        // Append the CRC and length to the correct location
        while ((ch = fgetc(fptr)) != EOF)
        {
            //printf("index: %i\n", index);

            // 16 is the offset to the end of the file
            if (ftell(target) == crcFileInfo->appendIndex)
            {
                // Build the string
                writeFrame(crcFileInfo, target);

                fseek(source, 18, SEEK_CUR);
            }
            else
            {
                fputc(ch, target);
            }
        }*/
    }

    //fclose(fptr);
}

void MenuConfigComponent::getAllFields(FILE* fp)
{
    // Read contents from file
    char c;
    uint8_t lineBuffer[255] = "";
    uint8_t lineCount = 0u;
    uint8_t lineIndex = 0u;
    std::memset(&asciiConfigSettings, 0u, sizeof(asciiConfigSettings));
    c = fgetc(fp);
    while (c != EOF)
    {
        c = fgetc(fp);
        lineBuffer[lineIndex] = c;
        ++lineIndex;

        if (c == '\n')
        {
            printf("line buffer: ");
            for (int i = 0; i < sizeof(lineBuffer); ++i)
                printf("%c", lineBuffer[i]);

            lineIndex = 0;
            // Line is over, parse the line and store the stuff
            decipherInfo(fp, lineBuffer, lineCount);
            memset(lineBuffer, 0u, sizeof(lineBuffer));

            // Increment the line count
            ++lineCount;
        }
    }

}

void MenuConfigComponent::decipherInfo(FILE* fptr, uint8_t* info, uint8_t lineCount)
{
    // Get the info and store it locally
    uint8_t infoHold[MIR_CONFIG_RECORD_MAX_SIZE] = "";

    switch (lineCount)
    {
    case MIR_RECORD_OPERATING_MODE:
        asciiConfigSettings.indicies[MIR_RECORD_OPERATING_MODE] = ftell(fptr);
        getRecordInformation(info, (uint8_t*)asciiConfigSettings.operatingMode, lineCount);
        break;
    case MIR_RECORD_CRC_INFO:
        asciiConfigSettings.indicies[MIR_RECORD_CRC_INFO] = ftell(fptr);
        getRecordInformation(info, (uint8_t*)asciiConfigSettings.crcFunctionPtr, lineCount);
        break;
    case MIR_RECORD_BYTE_ORDER:
        asciiConfigSettings.indicies[MIR_RECORD_BYTE_ORDER] = ftell(fptr);
        getRecordInformation(info, (uint8_t*)asciiConfigSettings.byteOrder, lineCount);
        break;
    case MIR_RECORD_APPENDAGE_ADDRESS:
        asciiConfigSettings.indicies[MIR_RECORD_APPENDAGE_ADDRESS] = ftell(fptr);
        getRecordInformation(info, (uint8_t*)asciiConfigSettings.appendAddress, lineCount);
        break;
    case MIR_RECORD_FILE_LOCATION:
        asciiConfigSettings.indicies[MIR_RECORD_FILE_LOCATION] = ftell(fptr);
        getRecordInformation(info, (uint8_t*)asciiConfigSettings.fileLocation, lineCount);
        break;
    case MIR_RECORD_FILE_NAME:
        asciiConfigSettings.indicies[MIR_RECORD_FILE_NAME] = ftell(fptr);
        getRecordInformation(info, (uint8_t*)asciiConfigSettings.fileName, lineCount);
        break;
    case MIR_RECORD_FILE_OUTPUT_NAME:
        asciiConfigSettings.indicies[MIR_RECORD_FILE_OUTPUT_NAME] = ftell(fptr);
        getRecordInformation(info, (uint8_t*)asciiConfigSettings.ouputFileName, lineCount);
        break;
    case MIR_RECORD_FILE_OUTPUT_LOCATION:
        asciiConfigSettings.indicies[MIR_RECORD_FILE_OUTPUT_LOCATION] = ftell(fptr);
        getRecordInformation(info, (uint8_t*)asciiConfigSettings.outputLocation, lineCount);
        break;
    default:
        break;
    }
}

void MenuConfigComponent::getRecordInformation(uint8_t* info, uint8_t* infoToBeStored, uint8_t lineIndex)
{
    bool infoFlag = false;
    uint8_t infoIndex = 0u;

    for (int i = 0; i < MIR_CONFIG_RECORD_MAX_SIZE; ++i)
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
        if (info[i] == 0x3A)
        {
            infoFlag = true;
        }
    }

    for (int j = 0; j < MIR_CONFIG_RECORD_MAX_SIZE - 10; ++j)
    {
        printf("%c", infoToBeStored[j]);
    }
    printf("\n");
}

void MenuConfigComponent::convASCIItoHex(uint8_t* ASCIIBuffer, uint8_t* hexBufferPacked, uint64_t size)
{

    uint8_t num = 0;
    uint8_t hexBuffMe[256] = "";
    uint8_t ASCIIChar = 0;
    uint8_t hexCounter = 0;
    uint8_t bufferIndex = 0;


    for (uint8_t i = 0; i <= size; ++i)
    {
        ASCIIChar = ASCIIBuffer[i];
        if (hexCounter == 1)
        {
            num <<= 4;
            if (ASCIIChar <= '9')
            {
                num += (ASCIIChar - '0');
            }
            else
            {
                num += (ASCIIChar - 'A' + 10);
            }
        }
        else
        {
            if (ASCIIChar <= '9')
            {
                num = (ASCIIChar - '0');
            }
            else
            {
                num = (ASCIIChar - 'A' + 10);
            }
        }
        ++hexCounter;
        if (hexCounter == 2)
        {
            hexBuffMe[bufferIndex] = num;
            ++bufferIndex;
            num = 0;
            hexCounter = 0;
        }
    }
    for (uint8_t j = 0; j <= (size / 2); ++j)
    {
        hexBufferPacked[j] = hexBuffMe[j];

    }
    memset(hexBuffMe, 0, sizeof(hexBuffMe));
}


void MenuConfigComponent::retrieveConfigurationSettings(void)
{
    // Assign the CRC function pointer
    uint8_t crcFunc[2] = "";
    convASCIItoHex((uint8_t*)asciiConfigSettings.crcFunctionPtr, crcFunc, 2);
    crcFunctionPtr = crcFunc[0];

    // Assign the mode
    uint8_t opMode[2] = "";
    convASCIItoHex((uint8_t*)asciiConfigSettings.operatingMode, opMode, 2);
    operatingMode = opMode[0];

    // Assign the byte order
    uint8_t byteLOrder[2] = "";
    convASCIItoHex((uint8_t*)asciiConfigSettings.byteOrder, byteLOrder, 2);
    byteOrder = byteLOrder[0];

}

void MenuConfigComponent::convHex2ASCII(uint8_t source, char* dest)
{
    dest[0] = lookupTableASCII(source >> 4);
    dest[1] = lookupTableASCII(source & 0x0F);;
}

char MenuConfigComponent::lookupTableASCII(uint8_t nibble)
{
    char retVal = 0;

    switch (nibble)
    {
    case 0x00:
        retVal = '0';
        break;
    case 0x01:
        retVal = '1';
        break;
    case 0x02:
        retVal = '2';
        break;
    case 0x03:
        retVal = '3';
        break;
    case 0x04:
        retVal = '4';
        break;
    case 0x05:
        retVal = '5';
        break;
    case 0x06:
        retVal = '6';
        break;
    case 0x07:
        retVal = '7';
        break;
    case 0x08:
        retVal = '8';
        break;
    case 0x09:
        retVal = '9';
        break;
    case 0x0A:
        retVal = 'A';
        break;
    case 0x0B:
        retVal = 'B';
        break;
    case 0x0C:
        retVal = 'C';
        break;
    case 0x0D:
        retVal = 'D';
        break;
    case 0x0E:
        retVal = 'E';
        break;
    case 0x0F:
        retVal = 'F';
        break;
    default:
        retVal = '0';
        break;
    }

    return retVal;
}

void appendField(FILE* fp, MIR_CONFIG_FILE_RECORD_TYPES record)
{
   /* switch(record)
    {
    case MIR_RECORD_OPERATING_MODE:
        // write
        fwrite(inputBuffer, 1, 5, fp);
        // seek
        break;
    case MIR_RECORD_CRC_INFO:
        break;
    case MIR_RECORD_BYTE_ORDER:
        break;
    case MIR_RECORD_APPENDAGE_ADDRESS:
        break;
    case MIR_RECORD_FILE_LOCATION:
        break;
    case MIR_RECORD_FILE_NAME:
        break;
    case MIR_RECORD_FILE_OUTPUT_NAME:
        break;
    case MIR_RECORD_FILE_OUTPUT_LOCATION:
        break;
    default:
        break;
        
    }*/
}
