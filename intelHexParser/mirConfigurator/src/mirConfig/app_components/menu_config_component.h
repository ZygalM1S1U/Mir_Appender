#ifndef _MENU_CONFIG_COMPONENT_H__
#define _MENU_CONFIG_COMPONENT_H__

#include <string>
#include "msg_event.h"
#include "app_component.h"

class MenuConfigComponent : public AppComponent
{
public:

	/// @brief  constructor 
    MenuConfigComponent();

	/// @brief  destructor 
    virtual ~MenuConfigComponent();

	/// @brief  update GUI widgets
    virtual void draw(float elapsedTime);

    float getHeight();

protected:

    void pullAllFileAttributes(void);

    void getAllFields(FILE* fp);

    void getRecordInformation(uint8_t* info, uint8_t* infoToBeStored, uint8_t lineIndex);

    void decipherInfo(FILE* fptr, uint8_t* info, uint8_t lineCount);

    void convASCIItoHex(uint8_t* ASCIIBuffer, uint8_t* hexBufferPacked, uint64_t size);

    void retrieveConfigurationSettings(void);

    void editAllFileAttributes(void);

    void convHex2ASCII(uint8_t source, char* dest);

    char lookupTableASCII(uint8_t nibble);
    
    //void appendField(FILE* fp, MIR_CONFIG_FILE_RECORD_TYPES record);


    std::string displayState;

    float height;
	
	bool loggingOn;

    char inputBuffer[25];
    char inputFileNameBuffer[256];
    char inputFileLocBuffer[256];
    char inputFileOpLocBuffer[256];
    char inputFileOpNameBuffer[256];
    bool first;
};

#endif
