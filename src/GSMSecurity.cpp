/*
  This file is part of the MKR GSM library.
  Copyright (C) 2018 Darren Jeacocke

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "GSMSecurity.h"

enum {
  SSL_ROOT_CERT   = 0,
  SSL_CLIENT_CERT = 1,
  SSL_PRIVATE_KEY = 2
};

GSMSecurity::GSMSecurity() :
  _id(0),
  _sslValidation(SSL_VALIDATION_NONE),
  _sslVersion(SSL_VERSION_ANY),
  _sslCipher(SSL_CIPHER_AUTO)
{
  MODEM.addUrcHandler(this);
}

GSMSecurity::~GSMSecurity()
{
  MODEM.removeUrcHandler(this);
}

void GSMSecurity::begin()
{
  MODEM.begin();
}

int GSMSecurity::setValidation(int val)
{
  _sslValidation = val;
  _id = 0;
  MODEM.sendf("AT+USECPRF=%d,0,%d", _id, _sslValidation);
  if (MODEM.waitForResponse() != 1) {
    return 0;
  }

  return 1;
}

int GSMSecurity::setVersion(int val)
{
  _sslVersion = val;
  MODEM.sendf("AT+USECPRF=%d,1,%d", _id, _sslVersion);
  if (MODEM.waitForResponse() != 1) {
    return 0;
  }

  return 1;
}

int GSMSecurity::setCipher(int val)
{
  _sslCipher = val;
  MODEM.sendf("AT+USECPRF=%d,2,%d", _id, _sslCipher);
  if (MODEM.waitForResponse() != 1) {
    return 0;
  }

  return 1;
}

// TODO: implement this
int GSMSecurity::listAllCertificates()
{
  MODEM.sendf("AT+USECMNG=3");
  if (MODEM.waitForResponse() != 1) {
    return 0;
  }

  return 1;
}

int GSMSecurity::removeAllCertificates()
{
  MODEM.sendf("AT+USECMNG=2,0,\"ROOT_CERT\"");
  MODEM.waitForResponse();
  MODEM.sendf("AT+USECMNG=2,1,\"CLIENT_CERT\"");
  MODEM.waitForResponse();
  MODEM.sendf("AT+USECMNG=2,2,\"PRIVATE_KEY\"");
  if (MODEM.waitForResponse() != 1) {
    return 0;
  }

  return 1;
}

int GSMSecurity::setRootCertificate(const char* cert)
{
  return setCertificate(SSL_ROOT_CERT, "ROOT_CERT", cert);
}

int GSMSecurity::setClientCertificate(const char* cert)
{
  return setCertificate(SSL_CLIENT_CERT, "CLIENT_CERT", cert);
}

int GSMSecurity::setPrivateKey(const char* cert)
{
  return setCertificate(SSL_PRIVATE_KEY, "PRIVATE_KEY", cert);
}

int GSMSecurity::setCertificate(int type, const char* name, const char* cert)
{
  int i = 0;
  while (i <= MAX_CERTS) {
    if (_certs[i].type == -1) {
      break;
    }
    i++;
  }

  if (i == MAX_CERTS) {
    return -2;
  }

  _certs[i].name = name;
  _certs[i].type = type;

  MODEM.sendf("AT+USECMNG=0,%d,\"%s\",%d", type, name, strlen(cert) + 2);
  MODEM.waitForResponse(100);
  MODEM.send(cert);
  MODEM.waitForResponse(1000);

  if (_certs[i].hash == "") {
    return 0;
  }

  int opCode;
  switch (type) {
  case SSL_ROOT_CERT:
    opCode = 3;
    break;
  case SSL_CLIENT_CERT:
    opCode = 5;
    break;
  case SSL_PRIVATE_KEY:
    opCode = 6;
    break;
  default:
    return -1;
  }

  MODEM.sendf("AT+USECPRF=%d,%d,\"%s\"", _id, opCode, name);
  return MODEM.waitForResponse();
}

void GSMSecurity::handleUrc(const String& urc)
{
  if (urc.startsWith("+USECMNG: 0,")) {
    String temp = urc;

    int certType = urc.charAt(12) - '0';
    String certName;
    String certHash;

    int firstQuoteIndex = temp.indexOf('"');
    int secondQuoteIndex = temp.indexOf('"', firstQuoteIndex + 1);
    int thirdQuoteIndex = temp.indexOf('"', secondQuoteIndex + 1);
    int lastQuoteIndex = temp.lastIndexOf('"');

    if (firstQuoteIndex != -1 && secondQuoteIndex != -1 && firstQuoteIndex != secondQuoteIndex) {
      certName = temp.substring(firstQuoteIndex + 1, secondQuoteIndex);
    }
    if (thirdQuoteIndex != -1 && lastQuoteIndex != -1 && thirdQuoteIndex != lastQuoteIndex) {
      certHash = temp.substring(thirdQuoteIndex + 1, lastQuoteIndex);
    }

    for (int i = 0; i < MAX_CERTS; i++) {
      if (_certs[i].type == certType && _certs[i].name == certName ) {
        _certs[i].hash = certHash;
        break;
      }
    }
  }
}
