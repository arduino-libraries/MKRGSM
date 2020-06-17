/*
  This file is part of the MKR GSM library.
  Copyright (C) 2017  Arduino AG (http://www.arduino.cc/)

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

#ifndef _GSM_SSL_CLIENT_H_INCLUDED
#define _GSM_SSL_CLIENT_H_INCLUDED

#include "GSMClient.h"
#include "utility/GSMRootCerts.h"
class GSMSSLClient : public GSMClient {

public:
  GSMSSLClient(bool synch = true);
  virtual ~GSMSSLClient();

  virtual int ready();

  virtual int connect(IPAddress ip, uint16_t port);
  virtual int connect(const char* host, uint16_t port);
  virtual void setSignedCertificate(const uint8_t* cert, const char* name, size_t size);
  virtual void setPrivateKey(const uint8_t* key, const char* name, size_t size);
  virtual void useSignedCertificate(const char* name);
  virtual void usePrivateKey(const char* name);
  virtual void setTrustedRoot(const char* name);
  virtual void setUserRoots(const GSMRootCert * userRoots, size_t size);
  virtual void eraseTrustedRoot();

private:
  static bool _rootCertsLoaded;
  int _certIndex;
  int _state;
  const GSMRootCert * _gsmRoots;
  int _sizeRoot;

};

#endif
