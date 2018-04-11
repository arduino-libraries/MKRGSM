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

#ifndef _GSM_SECURITY_H_INCLUDED
#define _GSM_SECURITY_H_INCLUDED

#include <Arduino.h>

#include "Modem.h"

enum {
  SSL_VERSION_ANY     = 0,
  SSL_VERSION_TLS_1_0 = 1,
  SSL_VERSION_TLS_1_1 = 2,
  SSL_VERSION_TLS_1_2 = 3
};

enum {
  SSL_VALIDATION_NONE                           = 0,
  SSL_VALIDATION_ROOT_CERT                      = 1,
  SSL_VALIDATION_ROOT_CERT_URL_CHECK            = 2,
  SSL_VALIDATION_ROOT_CERT_URL_CHECK_DATE_CHECK = 3
};

enum {
  SSL_CIPHER_AUTO                                = 0,
  SSL_CIPHER_TLS_RSA_WITH_AES_128_CBC_SHA        = 1,
  SSL_CIPHER_TLS_RSA_WITH_AES_128_CBC_SHA256     = 2,
  SSL_CIPHER_TLS_RSA_WITH_AES_256_CBC_SHA        = 3,
  SSL_CIPHER_TLS_RSA_WITH_AES_256_CBC_SHA256     = 4,
  SSL_CIPHER_TLS_RSA_WITH_3DES_EDE_CBC_SHA       = 5,
  SSL_CIPHER_TLS_PSK_WITH_AES_128_CBC_SHA        = 6,
  SSL_CIPHER_TLS_PSK_WITH_AES_256_CBC_SHA        = 7,
  SSL_CIPHER_TLS_PSK_WITH_3DES_EDE_CBC_SHA       = 8,
  SSL_CIPHER_TLS_RSA_PSK_WITH_AES_128_CBC_SHA    = 9,
  SSL_CIPHER_TLS_RSA_PSK_WITH_AES_256_CBC_SHA    = 10,
  SSL_CIPHER_TLS_RSA_PSK_WITH_3DES_EDE_CBC_SHA   = 11,
  SSL_CIPHER_TLS_PSK_WITH_AES_128_CBC_SHA256     = 12,
  SSL_CIPHER_TLS_PSK_WITH_AES_256_CBC_SHA384     = 13,
  SSL_CIPHER_TLS_RSA_PSK_WITH_AES_128_CBC_SHA256 = 14,
  SSL_CIPHER_TLS_RSA_PSK_WITH_AES_256_CBC_SHA384 = 15
};

class GSMSecurity : public ModemUrcHandler {

public:
  GSMSecurity();
  virtual ~GSMSecurity();

  operator int() { return _id; }

  /** Check modem response and restart it
   */
  void begin();
  void handleUrc(const String& urc);

  int listAllCertificates();
  int removeAllCertificates();
  int setValidation(int val);
  int setVersion(int val);
  int setCipher(int val);
  int setRootCertificate(const char* cert);
  int setClientCertificate(const char* cert);
  int setPrivateKey(const char* cert);

private:
  int setCertificate(int type, const char* name, const char* cert);

  // TODO: make _id autoincrement
  int _id;
  int _sslValidation;
  int _sslVersion;
  int _sslCipher;
  const char* _sslServerHostname;

  #define MAX_CERTS 3
  struct {
    int type = -1;
    String name;
    String hash;
  } _certs[MAX_CERTS];
};

#endif
