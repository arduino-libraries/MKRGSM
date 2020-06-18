#pragma once

#include <Arduino.h>


class GSMFileUtils {
public:
    GSMFileUtils(bool debug = false);

    bool begin(const unsigned long timeout);
    bool begin() { return begin(10000); };
    size_t fileCount() const { return _count; };
    size_t listFiles(String list[]) const;
    size_t listFile(const String filename) const;

    void downloadFile(const String filename, const char buf[], const size_t size, const bool binary, const bool append);
    void downloadFile(const String filename, const char buf[], const size_t size) { downloadFile(filename, buf, size, false, true); };
    void downloadFile(const String filename, const String& buf, const bool binary = false, const bool append = false) { downloadFile(filename, buf.c_str(), buf.length(), binary, append); }

    void appendFile(const String filename, const String& buf)                   { downloadFile(filename, buf.c_str(), buf.length(), false, true); }
    void appendFile(const String filename, const char buf[], const size_t size) { downloadFile(filename, buf, size, false, true); }
    
    void downloadBinary(const String filename, const char buf[], const size_t size, const bool append = false) { downloadFile(filename, buf, size, true, append); }
    void downloadBinary(const String filename, const String& buf, const bool append = false)                   { downloadFile(filename, buf.c_str(), buf.length(), true, append); }

    int deleteFile(const String filename);
    int deleteFiles();

    size_t readFile(const String filename, String* content, const bool binary = false);
    size_t readFile(const String filename, uint8_t* content, const bool binary = true);
    size_t readBinary(const String filename, uint8_t* content);

    size_t freeSpace();

private:
    int _count;
    String _files;

    bool _debug;

    void _countFiles();
    int _getFileList();

};

void printFiles(const GSMFileUtils fileUtils);