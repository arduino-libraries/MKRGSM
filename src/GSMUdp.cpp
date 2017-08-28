#include <Modem.h>

#include "GSMUdp.h"

GSMUDP::GSMUDP() :
  _socket(-1),
  _txIp((uint32_t)0),
  _txHost(NULL),
  _txPort(0),
  _txSize(0),
  _rxIp((uint32_t)0),
  _rxPort(0),
  _rxSize(0),
  _rxIndex(0)
{
}

uint8_t GSMUDP::begin(uint16_t port)
{
  String response;

  MODEM.send("AT+USOCR=17");

  if (MODEM.waitForResponse(100, &response) != 1) {
    return 0;
  }

  _socket = response.charAt(response.length() - 1) - '0';

  String command;
  command.reserve(16);

  command += "AT+USOLI=";
  command += _socket;
  command += ",";
  command += port;

  MODEM.send(command);
  if (MODEM.waitForResponse(10000) != 1) {
    stop();
    return 0;
  }

  return 1;
}

void GSMUDP::stop()
{
  if (_socket < 0) {
    return;
  }

  String command;
  command.reserve(10);

  command += "AT+USOCL=";
  command += _socket;

  MODEM.send(command);
  MODEM.waitForResponse(10000);

  _socket = -1;
}

int GSMUDP::beginPacket(IPAddress ip, uint16_t port)
{
  _txIp = ip;
  _txHost = NULL;
  _txPort = port;
  _txSize = 0;

  return 1;
}

int GSMUDP::beginPacket(const char *host, uint16_t port)
{
  _txIp = (uint32_t)0;
  _txHost = host;
  _txPort = port;
  _txSize = 0;

  return 1;
}

int GSMUDP::endPacket()
{
  String command;

  if (_txHost != NULL) {
    command.reserve(26 + strlen(_txHost) + _txSize * 2);
  } else {
    command.reserve(41 + _txSize * 2);
  }

  command += "AT+USOST=";
  command += _socket;
  command += ",\"";

  if (_txHost != NULL) {
    command += _txHost;
  } else {
    command += _txIp[0];
    command += '.';
    command += _txIp[1];
    command += '.';
    command += _txIp[2];
    command += '.';
    command += _txIp[3];
  }

  command += "\",";
  command += _txPort;
  command += ",",
  command += _txSize;
  command += ",\"";

  for (size_t i = 0; i < _txSize; i++) {
    byte b = _txBuffer[i];

    byte n1 = (b >> 4) & 0x0f;
    byte n2 = (b & 0x0f);

    command += (char)(n1 > 9 ? 'A' + n1 - 10 : '0' + n1);
    command += (char)(n2 > 9 ? 'A' + n2 - 10 : '0' + n2);
  }

  command += "\"";

  MODEM.send(command);

  if (MODEM.waitForResponse() == 1) {
    return 1;
  } else {
    return 0;
  }
}

size_t GSMUDP::write(uint8_t b)
{
  return write(&b, sizeof(b));
}

size_t GSMUDP::write(const uint8_t *buffer, size_t size)
{
  size_t spaceAvailable = sizeof(_txBuffer) - _txSize;

  if (size > spaceAvailable) {
    size = spaceAvailable;
  }

  memcpy(&_txBuffer[_txSize], buffer, size);
  _txSize += size;

  return size;
}

int GSMUDP::parsePacket()
{
  String response;
  String command;
  command.reserve(14);

  command += "AT+USORF=";
  command += _socket;
  command += ",";
  command += sizeof(_rxBuffer);

  MODEM.send(command);

  if (MODEM.waitForResponse(10000, &response) != 1) {
    return 0;
  }

  if (!response.startsWith("+USORF: ")) {
    return 0;
  }

  response.remove(0, 11);

  int firstQuoteIndex = response.indexOf('"');
  if (firstQuoteIndex == -1) {
    return 0;
  }

  String ip = response.substring(0, firstQuoteIndex);
  _rxIp.fromString(ip);

  response.remove(0, firstQuoteIndex + 2);

  int firstCommaIndex = response.indexOf(',');
  if (firstCommaIndex == -1) {
    return 0;
  }

  String port = response.substring(0, firstCommaIndex);
  _rxPort = port.toInt();
  firstQuoteIndex = response.indexOf("\"");

  response.remove(0, firstQuoteIndex + 1);
  response.remove(response.length() - 1);

  _rxIndex = 0;
  _rxSize = response.length() / 2;

  for (size_t i = 0; i < _rxSize; i++) {
    byte n1 = response[i * 2];
    byte n2 = response[i * 2 + 1];

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

    _rxBuffer[i] = (n1 << 4) | n2;
  }

  return _rxSize;
}

int GSMUDP::available()
{
  return (_rxIndex - _rxSize);
}

int GSMUDP::read()
{
  byte b;

  if (read(&b, sizeof(b)) == 1) {
    return b;
  }

  return -1;
}

int GSMUDP::read(unsigned char* buffer, size_t len)
{
  size_t readMax = available();

  if (len > readMax) {
    len = readMax;
  }

  memcpy(buffer, &_rxBuffer[_rxIndex], len);

  _rxIndex += len;

  return len;
}

int GSMUDP::peek()
{
  if (available() > 1) {
    return _rxBuffer[_rxIndex];
  }

  return -1;
}

void GSMUDP::flush()
{
}

IPAddress GSMUDP::remoteIP()
{
  return _rxIp;
}

uint16_t GSMUDP::remotePort()
{
  return _rxPort;
}
