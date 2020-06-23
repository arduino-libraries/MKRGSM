#pragma once

#include <Arduino.h>


class GSMFileUtils {
public:
    GSMFileUtils(bool debug = false);

    bool begin(const bool restart);
    bool begin() { return begin(true); };

    uint32_t fileCount() const { return _count; };
    size_t listFiles(String list[]) const;
    uint32_t listFile(const String filename) const;

    uint32_t downloadFile(const String filename, const char buf[], const uint32_t size, const bool append);
    uint32_t downloadFile(const String filename, const char buf[], const uint32_t size) { return downloadFile(filename, buf, size, false); };
    uint32_t downloadFile(const String filename, const String& buf) { return downloadFile(filename, buf.c_str(), buf.length(), false); }

    uint32_t appendFile(const String filename, const String& buf)                     { return downloadFile(filename, buf.c_str(), buf.length(), true); }
    uint32_t appendFile(const String filename, const char buf[], const uint32_t size) { return downloadFile(filename, buf, size, true); }
    
    bool deleteFile(const String filename);
    int deleteFiles();

    uint32_t readFile(const String filename, String* content);
    uint32_t readFile(const String filename, uint8_t* content);
    uint32_t readBlock(const String filename, const uint32_t offset, const uint32_t len, uint8_t* content);

    uint32_t freeSpace();

private:
    int _count;
    String _files;

    bool _debug;

    void _countFiles();
    int _getFileList();

};

void printFiles(const GSMFileUtils fileUtils);