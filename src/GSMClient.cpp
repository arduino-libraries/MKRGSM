#include "GSMClient.h"

GSMClient::GSMClient(bool synch) :
  GSMClient(-1, synch)
{
}

GSMClient::GSMClient(int socket, bool synch) :
  _synch(synch),
  _socket(socket),
  _rxIndex(0)
{
   MODEM.addUrcHandler(this);
}

GSMClient::~GSMClient()
{
  MODEM.removeUrcHandler(this);
}

int GSMClient::ready()
{
  return MODEM.ready();
}

int GSMClient::connect(IPAddress ip, uint16_t port)
{
  return connect(ip, port, false);
}

int GSMClient::connectSSL(IPAddress ip, uint16_t port)
{
  return connect(ip, port, true);
}

int GSMClient::connect(const char *host, uint16_t port)
{
  return connect(host, port, false);
}

int GSMClient::connectSSL(const char *host, uint16_t port)
{
  return connect(host, port, true);
}

int GSMClient::connect(IPAddress ip, uint16_t port, bool ssl)
{
  String host;
  host.reserve(16);

  host += ip[0];
  host += '.';
  host += ip[1];
  host += '.';
  host += ip[2];
  host += '.';
  host += ip[3];

  connect(host.c_str(), port, ssl);
}

int GSMClient::connect(const char *host, uint16_t port, bool ssl)
{
  String command;
  String response;

  MODEM.send("AT+USOCR=6");
  if (MODEM.waitForResponse(100, &response) != 1) {
    return 2;
  }

  if (!response.startsWith("+USOCR: ")) {
    return 2;
  }

  _socket = response.charAt(response.length() - 1) - '0';

  command.reserve(19 + strlen(host));

  if (ssl) {
    command += "AT+USOSEC=";
    command += _socket;
    command +=  ",1";

    MODEM.send(command);
    if (MODEM.waitForResponse(10000, &response) != 1) {
      stop();
      return 2;
    }
  }

  command = "";
  command += "AT+USOCO=";
  command += _socket;
  command += ",\"";
  command += host;
  command += "\",";
  command += port;

  MODEM.send(command);
  if (MODEM.waitForResponse(20000) != 1) {
    stop();
    return 2;
  }

  return 1;
}

void GSMClient::beginWrite(bool sync)
{
}

size_t GSMClient::write(uint8_t c)
{
  return write(&c);
}

size_t GSMClient::write(const uint8_t *buf)
{
  return write(buf, strlen((const char*)buf));
}

size_t GSMClient::write(const uint8_t* buf, size_t size)
{
  if (_socket == -1) {
    return 0;
  }

  if (size > 512) {
    size = 512;
  }

  String command;
  command.reserve(19 + size * 2);

  command += "AT+USOWR=";
  command += _socket;
  command += ",";
  command += size;
  command += ",\"";

  for (size_t i = 0; i < size; i++) {
    byte b = buf[i];

    byte n1 = (b >> 4) & 0x0f;
    byte n2 = (b & 0x0f);

    command += (char)(n1 > 9 ? 'A' + n1 - 10 : '0' + n1);
    command += (char)(n2 > 9 ? 'A' + n2 - 10 : '0' + n2);
  }

  command += "\"";
  
  MODEM.send(command);
  if (MODEM.waitForResponse(10000) != 1) {
    return 0;
  }

  return size;
}

void GSMClient::endWrite(bool sync)
{
}

uint8_t GSMClient::connected()
{
  MODEM.poll();

  if (_socket == -1) {
    return 0;
  }

  return 1;
}

GSMClient::operator bool()
{
  return (_socket != -1);
}

int GSMClient::read(uint8_t *buf, size_t size)
{
  int canRead = available();

  if (size > canRead) {
    size = canRead;
  }

  for (int i = 0; i < size; i++) {
    byte n1 = _rxBuffer[_rxIndex];
    byte n2 = _rxBuffer[_rxIndex + 1];

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

    buf[i] = (n1 << 4) | n2;

    _rxIndex += 2;
  }

  return size;
}

int GSMClient::read()
{
  byte b;

  if (read(&b, 1) == 1) {
    return b;
  }

  return -1;
}

int GSMClient::available()
{
  if (_socket == -1) {
    return 0;
  }

  if (_rxBuffer.length() > _rxIndex) {
    return (_rxBuffer.length() - _rxIndex)  / 2;
  }

  String command;
  command.reserve(14);

  command += "AT+USORD=";
  command += _socket;
  command += ",512";

  MODEM.send(command);

  if (MODEM.waitForResponse(10000, &_rxBuffer) == 1) {
    if (_rxBuffer.startsWith("+USORD: ")) {
      int firstQuoteIndex = _rxBuffer.indexOf("\"");

      _rxBuffer.remove(0, firstQuoteIndex + 1);
      _rxBuffer.remove(_rxBuffer.length() - 1);

      _rxIndex = 0;
      return _rxBuffer.length() / 2;
    }
  } else {
    _socket = -1;
  }

  return 0;
}

int GSMClient::peek()
{
  if (available() < 1) {
    return -1;
  }

  byte n1 = _rxBuffer[_rxIndex];
  byte n2 = _rxBuffer[_rxIndex + 1];

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

  return ((n1 << 4) | n2);
}

void GSMClient::flush()
{
}

void GSMClient::stop()
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

void GSMClient::handleUrc(const String& urc)
{
  if (urc.startsWith("+UUSOCL: ")) {
    int socket = urc.charAt(urc.length() - 1) - '0';

    if (socket == _socket) {
      // this socket closed
      _socket = -1;
    }
  } else if (urc.startsWith("+UUSORD: ") && urc.endsWith(",4294967295")) {
    // SSL disconnect
    int socket = urc.charAt(9) - '0';

    if (socket == _socket) {
      // this socket closed
      _socket = -1;
    }
  }
}
