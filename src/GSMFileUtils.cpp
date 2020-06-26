#include "Modem.h"

#include "GSMFileUtils.h"

GSMFileUtils::GSMFileUtils(bool debug)
    : _count(0)
    , _files("")
    , _debug(debug)
{
}

bool GSMFileUtils::begin(const bool restart)
{
    int status;

    if (restart)
        MODEM.begin();

    if (_debug) {
        MODEM.debug();
        MODEM.send("AT+CMEE=2");
        MODEM.waitForResponse();
    }

    for (unsigned long start = millis(); (millis() - start) < 10000;) {
        status = _getFileList();
        if (status == 1) {
            _countFiles();
            return true;
        }
        MODEM.poll();
    }
    return false;
}

int GSMFileUtils::_getFileList()
{
    String response;

    MODEM.send("AT+ULSTFILE=0");
    int status = MODEM.waitForResponse(5000, &response);
    if (!response.length())
        return -1;

    if (status) {
        String list = response.substring(11);
        list.trim();
        _files = list;
    }

    return status;
}

void GSMFileUtils::_countFiles()
{
    String list = _files;
    size_t len = 0;

    if (list.length() > 0) {
        for (int index = list.indexOf(','); index != -1; index = list.indexOf(',')) {
            list.remove(0, index + 1);
            ++len;
        }
        ++len;
    }
    _count = len;
}

size_t GSMFileUtils::listFiles(String files[]) const
{
    String list = _files;
    int index;

    if (_count == 0)
        return 0;

    size_t n = 0;

    for (index = list.indexOf(','); index != -1; index = list.indexOf(',')) {
        String file = list.substring(1, index - 1);
        files[n++] = file;
        list.remove(0, index + 1);
    }
    files[n++] = list.substring(1, list.lastIndexOf("\""));

    return n;
}

uint32_t GSMFileUtils::downloadFile(const String filename, const char buf[], uint32_t size, const bool append)
{
    if (!append)
        deleteFile(filename);

    MODEM.sendf("AT+UDWNFILE=\"%s\",%d", filename.c_str(), size * 2);
    MODEM.waitForPrompt(20000);

    char hex[size * 2] { 0 };

    for (auto i = 0; i < size; i++) {
        byte b = buf[i];

        byte n1 = (b >> 4) & 0x0f;
        byte n2 = (b & 0x0f);

        hex[i * 2] = (char)(n1 > 9 ? 'A' + n1 - 10 : '0' + n1);
        hex[i * 2 + 1] = (char)(n2 > 9 ? 'A' + n2 - 10 : '0' + n2);
    }
    for (auto h : hex)
        MODEM.write(h);

    int status = MODEM.waitForResponse(1000);
    if (status != 1)
        return 0;

    auto fileExists = _files.indexOf(filename) > 0;
    if (!fileExists) {
        _getFileList();
        _countFiles();
    }

    return size;
}

uint32_t GSMFileUtils::readFile(const String filename, String* content)
{
    String response;

    if (!listFile(filename)) {
        return 0;
    }

    MODEM.sendf("AT+URDFILE=\"%s\"", filename.c_str());
    MODEM.waitForResponse(1000, &response);

    size_t skip = 10;
    String _content = response.substring(skip);

    int commaIndex = _content.indexOf(',');
    skip += commaIndex;

    _content = _content.substring(commaIndex + 1);
    commaIndex = _content.indexOf(',');
    skip += commaIndex;

    String sizePart = _content.substring(0, commaIndex);
    uint32_t size = sizePart.toInt() / 2;
    skip += 3;

    String* _data = content;
    (*_data).reserve(size);

    for (auto i = 0; i < size; i++) {
        byte n1 = response[skip + i * 2];
        byte n2 = response[skip + i * 2 + 1];

        if (n1 > '9') {
            n1 = (n1 - 'A') + 10;
        } else {
            n1 = (n1 - '0');
        }

        if (n2 > '9') {
            n2 = (n2 - 'A') + 10;
        } else {
            n2 = (n2 - '0');
        }

        (*_data) += (char)((n1 << 4) | n2);
    }

    return (*_data).length();
}

uint32_t GSMFileUtils::readFile(const String filename, uint8_t* content)
{
    String response;

    if (listFile(filename) == 0) {
        return 0;
    }

    MODEM.sendf("AT+URDFILE=\"%s\"", filename.c_str());
    MODEM.waitForResponse(1000, &response);

    size_t skip = 10;
    String _content = response.substring(skip);

    int commaIndex = _content.indexOf(',');
    skip += commaIndex;

    _content = _content.substring(commaIndex + 1);
    commaIndex = _content.indexOf(',');
    skip += commaIndex;

    String sizePart = _content.substring(0, commaIndex);
    uint32_t size = sizePart.toInt() / 2;
    skip += 3;

    for (auto i = 0; i < size; i++) {
        byte n1 = response[skip + i * 2];
        byte n2 = response[skip + i * 2 + 1];

        if (n1 > '9') {
            n1 = (n1 - 'A') + 10;
        } else {
            n1 = (n1 - '0');
        }

        if (n2 > '9') {
            n2 = (n2 - 'A') + 10;
        } else {
            n2 = (n2 - '0');
        }

        content[i] = (n1 << 4) | n2;
    }

    return size;
}

uint32_t GSMFileUtils::readBlock(const String filename, const uint32_t offset, const uint32_t len, uint8_t* content)
{
    String response;

    if (listFile(filename) == 0) {
        return 0;
    }

    MODEM.sendf("AT+URDBLOCK=\"%s\",%d,%d", filename.c_str(), offset * 2, len * 2);
    MODEM.waitForResponse(1000, &response);

    size_t skip = 10;
    String _content = response.substring(skip);

    int commaIndex = _content.indexOf(',');
    skip += commaIndex;

    _content = _content.substring(commaIndex + 1);
    commaIndex = _content.indexOf(',');
    skip += commaIndex;

    String sizePart = _content.substring(0, commaIndex);
    uint32_t size = sizePart.toInt() / 2;
    skip += 3;

    for (auto i = 0; i < size; i++) {
        byte n1 = response[skip + i * 2];
        byte n2 = response[skip + i * 2 + 1];

        if (n1 > '9') {
            n1 = (n1 - 'A') + 10;
        } else {
            n1 = (n1 - '0');
        }

        if (n2 > '9') {
            n2 = (n2 - 'A') + 10;
        } else {
            n2 = (n2 - '0');
        }

        content[i] = (n1 << 4) | n2;
    }

    return size;
}

bool GSMFileUtils::deleteFile(const String filename)
{
    String response;

    if (listFile(filename) == 0)
        return false;

    MODEM.sendf("AT+UDELFILE=\"%s\"", filename.c_str());
    auto status = MODEM.waitForResponse(100, &response);

    if (status == 0)
        return false;

    _getFileList();
    _countFiles();

    return true;
}

int GSMFileUtils::deleteFiles()
{
    int n = 0;
    String files[_count];

    listFiles(files);

    while (_count > 0) {
        n += deleteFile(files[_count - 1]);
    }

    return n;
}

uint32_t GSMFileUtils::listFile(const String filename) const
{
    String response;
    int res;
    uint32_t size = 0;

    MODEM.sendf("AT+ULSTFILE=2,\"%s\"", filename.c_str());
    res = MODEM.waitForResponse(100, &response);
    if (res == 1) {
        String content = response.substring(11);
        size = content.toInt();
    }

    return size / 2;
}

uint32_t GSMFileUtils::freeSpace()
{
    String response;
    int res;
    uint32_t size = 0;

    MODEM.send("AT+ULSTFILE=1");
    res = MODEM.waitForResponse(100, &response);
    if (res == 1) {
        String content = response.substring(11);
        size = content.toInt();
    }

    return size;
}

void printFiles(const GSMFileUtils fu)
{
    auto count { fu.fileCount() };
    String files[count];

    Serial.print(count);
    Serial.print(count == 1 ? " file" : " files");
    Serial.println(" found.");

    fu.listFiles(files);

    for (auto f : files) {
        Serial.print("File ");
        Serial.print(f);
        Serial.print(" - Size: ");
        Serial.print(fu.listFile(f));
        Serial.println();
    }
}