/*
  Read large files block-by-block from the GSM module filesystem.

  This sketch allows you to read large (max 2.5MB) files from the
  module's internal filesystem using successive block-by-block reads.

  The contents of the file is printed to Serial port as an
  hexadecimal string which can be later converted to the original
  content using an external tools, such as 'xxd', eg. 

    'xxd -p -r sketck_output.txt data.bin'

  Circuit:
  - MKR GSM 1400 board

  Created 19 June 2020
  by Giampaolo Mancini

*/

#include <MKRGSM.h>

GSMFileUtils fileUtils(false);

// An existing file
constexpr char* filename { "update.bin" };

// Read bloack size
constexpr unsigned int blockSize { 512 };

void setup()
{
    Serial.begin(115200);
    while (!Serial)
        ;

    fileUtils.begin();

    auto size = fileUtils.listFile(filename);
    auto cycles = (size / blockSize) + 1;

    uint32_t totalRead { 0 };

    for (auto i = 0; i < cycles; i++) {
        uint8_t block[blockSize] { 0 };
        auto read = fileUtils.readBlock(filename, i * blockSize, blockSize, block);
        totalRead += read;
        for (auto j = 0; j < read; j++) {
            if (block[j] < 16)
                Serial.print(0);
            Serial.print(block[j], HEX);
        }
        Serial.println();
    }

    if (totalRead != size) {
        Serial.print("ERROR - File size: ");
        Serial.print(size);
        Serial.print(" Bytes read: ");
        Serial.println(totalRead);
    }
}

void loop()
{
}