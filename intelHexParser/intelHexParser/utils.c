#include "utils.h"

bool memCheck(void)
{

    bool retVal = false;
    int *sack;
    printf("Loading...\n");
    sack = malloc(1*sizeof(*sack));

    /* Paint a small portion of memory to check to see if dynamic memory allocation is operating OK */
    if(sack == NULL)
    {
        printf("Error, out of memory.\nMalloc failed.\n");
        free(sack);
        return 1;
    }
    else
    {
        retVal = true;
        printNewLine();
        int *temp = realloc(sack, 1*sizeof(int));
        if(temp != NULL)
        {
            sack = temp;
            free(sack);
        }
        else
        {
            free(sack);
            printf("Error, realloc unsuccessful.\nHeap error.\n");
            retVal = false;
        }
    }

    return retVal;
}

void initArray(Array* a, size_t initialSize)
{
    a->array = (int *)malloc(initialSize * sizeof(int));
    a->used = 0;
    a->size = initialSize;
}

void reallArray(Array* a, size_t initialSize)
{
    a->array = (int *)realloc(a->array, initialSize * sizeof(int));
    a->used = 0;
    a->size = initialSize;
}

void insertArray(Array* a, int element)
{
    if (a->used == a->size)
    {
        a->size *= 2;
        a->array = (int *)realloc(a->array, a->size * sizeof(int));
    }
    a->array[++a->used] = element;
}

void freeArray(Array* a)
{
    free(a->array);
    a->array = NULL;
    a->used = a->size = 0;
}

void convASCIItoHex(uint8_t* ASCIIBuffer, uint8_t* hexBufferPacked, uint64_t size)
{

    uint8_t num = 0;
    uint8_t* hexBuffMe = malloc(100/sizeof(uint8_t));
    uint8_t ASCIIChar = 0;
    uint8_t hexCounter = 0;
    uint8_t bufferIndex = 0;


    for(uint8_t i = 0; i <= size; ++i)
    {
        ASCIIChar = ASCIIBuffer[i];
        if(hexCounter == 1)
        {
            num <<= 4;
            if(ASCIIChar <= '9')
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
            if(ASCIIChar <= '9')
            {
                num = (ASCIIChar - '0');
            }
            else
            {
                num = (ASCIIChar - 'A' + 10);
            }
        }
        ++hexCounter;
        if(hexCounter == 2)
        {
            hexBuffMe[bufferIndex] = num;
            ++bufferIndex;
            num = 0;
            hexCounter = 0;
        }
    }
    for(uint8_t j = 0; j <= (size/2); ++j)
    {
        hexBufferPacked[j] = hexBuffMe[j];

    }
    memset(hexBuffMe, 0, sizeof(hexBuffMe));
}

uint16_t swapEndianness(uint8_t* data)
{
    return (data[0]>>8)|(data[1]<<8);
}

uint16_t crcCCITT16Calculate(const uint8_t* dataBuffer, const long size)
{
    uint16_t x = 0;
    uint16_t crc = 0xFFFF;
    long size_ = size;
    while(size_--)
    {
        x = crc>>8 ^ *dataBuffer++;
        x ^= x>>4;
        crc = (crc<<8) ^ ((uint16_t)(x<<12)) ^ ((uint16_t)(x<<5)) ^ ((uint16_t)x);
    }
    return crc;
}

void convHex2ASCII(uint8_t source, char* dest)
{
    dest[0] = lookupTableASCII(source >> 4);
    dest[1] = lookupTableASCII(source & 0x0F);;
}

char lookupTableASCII(uint8_t nibble)
{
    char retVal = 0;

    switch(nibble)
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

uint16_t crc16XModCalc(const uint8_t* data, const long size) {}

uint16_t crcIEC60335Calculate(const uint8_t* data, const long size)
{
    // Locals
    uint32_t calcCRC = 0u;
    uint16_t poly = 4129u;

    uint32_t inputData = 0;
    uint32_t counter = 0;

    uint16_t lowerNibble = 0;
    uint16_t upperNibble = 0;

    // Loop over all the bytes in the list - halfway though since we are
    // taking two at a time for the processing
    while (counter < (size / 2))
    {
        // Grab the bytes to be processed
        lowerNibble = data[2 * (int)counter];
        upperNibble = data[(2 * (int)counter) + 1];

        // Shift the upper a byte
        upperNibble <<= 8;

        // Combine the upper and lower byte into a word
        inputData = (uint32_t)(upperNibble | lowerNibble);

        // The prototol below for getting the CRC is modeled after the
        // AN4187 "Using the CRC peripheral in the STM32 family" document
        // which aligns with IEC 60335-1 and IEC 607030-1 standards (Class B)

        // XOR the input word with the running crc
        calcCRC = calcCRC ^ inputData;

        // Loop and shift and check for 1 in MSB
        for (int i = 0; i < 16; i++)
        {
            // Shift by one bit to left
            calcCRC = calcCRC << 1;

            // Check if the msb is set (bit 15 was set (now bit 16))
            // if it is then XOR with the polynomial while masking off left
            // most word as we need to keep a single word (16 bit)
            if ((calcCRC & 0x10000) == 0x10000)
                calcCRC = (calcCRC ^ poly) & 0xFFFF;
        }

        // Clean up before next loop
        lowerNibble = 0;
        upperNibble = 0;
        ++counter;
    }
    return (uint16_t)calcCRC;
}

uint8_t checksum2sComp(uint8_t* data, long size)
{
    uint8_t sum = 0u;

    for(long i = 0; i < size; ++i)
    {
        sum += data[i];
    }

    // modulo 256 sum
    sum %= 256;

    // twos complement
    return ~sum + 1;
}

void printString(char* string, long size)
{
    for(long i = 0; i < size; ++i)
    {
        printf("%c", string[i]);
    }
    printNewLine();
}

void printNewLine(void)
{
    printf("\n");
}
