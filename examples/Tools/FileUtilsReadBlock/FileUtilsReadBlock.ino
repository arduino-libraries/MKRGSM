#include <MKRGSM.h>

GSMFileUtils fileUtils(false);

constexpr char* filename { "update.bin" };
constexpr unsigned int blockSize { 512 };

void setup()
{
    Serial.begin(115200);
    while (!Serial)
        ;

    // Serial.println("Test readBlock.");

    fileUtils.begin();

    auto size = fileUtils.listFile(filename);
    // Serial.println(size);

    auto cycles = size / blockSize;
    auto spares = size % blockSize;

    for (auto i = 0; i < cycles; i++) {
        uint8_t block[blockSize] { 0 };
        fileUtils.readBlock(filename, i * blockSize, blockSize, block);
        for (auto j = 0; j < blockSize; j++) {
            if (block[j] < 16)
                Serial.print(0);
            Serial.print(block[j], HEX);
        }
        Serial.println();
    }

    uint8_t block[blockSize] { 0 };
    fileUtils.readBlock(filename, cycles * blockSize, spares, block);
    for (auto j = 0; j < spares; j++) {
        if (block[j] < 16)
            Serial.print(0);
        Serial.print(block[j], HEX);
    }
    Serial.println();
}

void loop()
{
}