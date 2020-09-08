/*
  This file is part of the MKR GSM library.
  Copyright (C) 2020  Arduino AG (http://www.arduino.cc/)

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

#include "Modem.h"

#include "GSMHttpUtils.h"

#define TRUST_ROOT_TYPE "CA,\""
#define CLIENT_CERT_TYPE "CC,\""
#define CLIENT_KEY_TYPE "PK,\""

bool GSMHttpUtils::_rootCertsLoaded = false;

GSMHttpUtils::GSMHttpUtils() :
  _gsmRoots(GSM_ROOT_CERTS),
  _sizeRoot(GSM_NUM_ROOT_CERTS),
  _httpresp(false),
  _ssl(false)
{
  MODEM.addUrcHandler(this);
}

GSMHttpUtils::~GSMHttpUtils()
{
  MODEM.removeUrcHandler(this);
}


void GSMHttpUtils::setSignedCertificate(const uint8_t* cert, const char* name, size_t size) {
  MODEM.sendf("AT+USECMNG=0,1,\"%s\",%d", name, size);
  MODEM.waitForResponse(1000);

  MODEM.write(cert, size);
  MODEM.waitForResponse(1000);
}

void GSMHttpUtils::setPrivateKey(const uint8_t* key, const char*name, size_t size) {

  MODEM.sendf("AT+USECMNG=0,2,\"%s\",%d", name, size);
  MODEM.waitForResponse(1000);
  MODEM.write(key, size);
  MODEM.waitForResponse(1000);
}

void GSMHttpUtils::setTrustedRoot(const char* name) {
  MODEM.sendf("AT+USECPRF=0,3,\"%s\"", name);
  MODEM.waitForResponse(100);
}

void GSMHttpUtils::useSignedCertificate(const char* name) {
  MODEM.sendf("AT+USECPRF=0,5,\"%s\"", name);
  MODEM.waitForResponse(100);
}

void GSMHttpUtils::usePrivateKey(const char* name) {
  MODEM.sendf("AT+USECPRF=0,6,\"%s\"", name);
  MODEM.waitForResponse(100);
}

void GSMHttpUtils::eraseTrustedRoot() {
  for(int i=0; i< _sizeRoot; i++) {
    eraseCert(_gsmRoots[i].name, 0);
  }
}

void GSMHttpUtils::eraseAllCertificates() {
  for (int cert_type = 0; cert_type < 3; cert_type++) {
    String response = "";
    MODEM.sendf("AT+USECMNG=3,%d", cert_type);
    MODEM.waitForResponse(100, &response);
    int index = 0;
    bool done = true;
    if(response != "") {
      while(done) {
        int index_tmp = response.indexOf("\r\n", index);
        String certname = "";
        if (index_tmp > 0) {
            certname = response.substring(index, index_tmp);
            index = index_tmp + 2;
        } else {
          certname = response.substring(index);
          done = false;
        }
        if(certname != "") {
          removeCertForType(certname, cert_type);
        }
      }
    }
  }
}

void GSMHttpUtils::removeCertForType(String certname, int type) {
int start_ind = -1;
int last_ind = 0;
  switch (type) {
    case 0:
      start_ind = certname.indexOf(TRUST_ROOT_TYPE) + sizeof(TRUST_ROOT_TYPE) - 1;
      break;
    case 1:
      start_ind = certname.indexOf(CLIENT_CERT_TYPE) + sizeof(CLIENT_CERT_TYPE) - 1;
      break;
    case 2:
      start_ind = certname.indexOf(CLIENT_KEY_TYPE) + sizeof(CLIENT_KEY_TYPE) - 1;
      break;
    default:
      break;
  }

  if (start_ind >= 0) {
    last_ind = certname.indexOf("\"",start_ind);
    eraseCert(certname.substring(start_ind, last_ind).c_str(), type);
  }
}

void GSMHttpUtils::eraseCert(const char* name, int type) {
  MODEM.sendf("AT+USECMNG=2,%d,\"%s\"", type, name);
  MODEM.waitForResponse(100);
}

void GSMHttpUtils::setUserRoots(const GSMRootCert * userRoots, size_t size) {
  _gsmRoots = userRoots;
  _sizeRoot = size;
}

void GSMHttpUtils::handleUrc(const String& urc)
{
  if (urc.startsWith("+UUHTTPCR: ")) {
      _httpresp = false;
      if (urc.endsWith(",1")) {
        _httpresp = true;
      }
     
  }
}

void GSMHttpUtils::enableSSL() {
  // Sets the profile ID #1
  MODEM.sendf("AT+USECPRF=0,0,1");
  MODEM.waitForResponse(100);
  _ssl = true;
}

void GSMHttpUtils::disableSSL() {
  if(_ssl) {
    // Sets the profile ID #0
    MODEM.sendf("AT+USECPRF=0,0,0");
    MODEM.waitForResponse(100);
  }
  _ssl = false;
}

void GSMHttpUtils::configServer(const char* url, int httpport) {

  // Reset the HTTP profile #0
  MODEM.send("AT+UHTTP=0");
  MODEM.waitForResponse(100);

  // Sets HTTP server name
  MODEM.sendf("AT+UHTTP=0,1,\"%s\"",url);
  MODEM.waitForResponse(100);

  // Sets HTTP server port
  MODEM.sendf("AT+UHTTP=0,5,%d",httpport);
  MODEM.waitForResponse(100);

  // Sets HTTP secure option
  if(_ssl) {
    MODEM.send("AT+UHTTP=0,6,1,0");
    MODEM.waitForResponse(100);
  }

  // DNS resolution of url
  MODEM.sendf("AT+UDNSRN=0,\"%s\"",url);
  MODEM.waitForResponse(10000);
}

void GSMHttpUtils::head(const char* path, const char* filename) {
  // Makes a HEAD request and store the resposne in _file
  MODEM.sendf("AT+UHTTPC=0,0,\"%s\",\"%s\"",path, filename);
  MODEM.waitForResponse(100);

}

void GSMHttpUtils::get(const char* path, const char* filename) {
  // Makes a GET request and store it in _file
  MODEM.sendf("AT+UHTTPC=0,1,\"%s\",\"%s\"",path, filename);
  MODEM.waitForResponse(100);

}

void GSMHttpUtils::del(const char* path, const char* filename) {
  // make a DELETE request and store it in _file
  MODEM.sendf("AT+UHTTPC=0,2,\"%s\",\"%s\"",path, filename);
  MODEM.waitForResponse(100);
}

void GSMHttpUtils::put(const char* path, const char* filename) {
  // make a PUT request and store it in _file
  MODEM.sendf("AT+UHTTPC=0,3,\"%s\",\"%s\"",path, filename);
  MODEM.waitForResponse(100);
}

void GSMHttpUtils::post(const char* path, const char* filename) {
  // make a POST request and store it in _file
  MODEM.sendf("AT+UHTTPC=0,4,\"%s\",\"%s\"",path, filename);
  MODEM.waitForResponse(100);
}


bool GSMHttpUtils::responseStatus() {
  MODEM.poll();
  return _httpresp;
}
