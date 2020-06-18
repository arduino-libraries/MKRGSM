#pragma once

#include <Arduino.h>


class GSMFileUtils {
public:
    GSMFileUtils(bool debug = false);

    bool begin(const unsigned long timeout);
    bool begin() { return begin(10000); };
    u_int32_t fileCount() const { return _count; };
    size_t listFiles(String list[]) const;
    u_int32_t listFile(const String filename) const;

    void downloadFile(const String filename, const char buf[], const uint32_t size, const bool binary, const bool append);
    void downloadFile(const String filename, const char buf[], const uint32_t size) { downloadFile(filename, buf, size, false, true); };
    void downloadFile(const String filename, const String& buf, const bool binary = false, const bool append = false) { downloadFile(filename, buf.c_str(), buf.length(), binary, append); }

    void appendFile(const String filename, const String& buf)                     { downloadFile(filename, buf.c_str(), buf.length(), false, true); }
    void appendFile(const String filename, const char buf[], const uint32_t size) { downloadFile(filename, buf, size, false, true); }
    
    void downloadBinary(const String filename, const char buf[], const uint32_t size, const bool append = false) { downloadFile(filename, buf, size, true, append); }
    void downloadBinary(const String filename, const String& buf, const bool append = false)                   { downloadFile(filename, buf.c_str(), buf.length(), true, append); }

    int deleteFile(const String filename);
    int deleteFiles();

    uint32_t readFile(const String filename, String* content, const bool binary = false);
    uint32_t readFile(const String filename, uint8_t* content, const bool binary = true);
    uint32_t readBinary(const String filename, uint8_t* content);
    // size_t readBlock(const String filename, String* content, const bool binary = false);
    uint32_t readBlock(const String filename, const uint32_t offset, const uint32_t len, uint8_t* content, const bool binary = true);

    uint32_t freeSpace();

private:
    int _count;
    String _files;

    bool _debug;

    void _countFiles();
    int _getFileList();

};

void printFiles(const GSMFileUtils fileUtils);