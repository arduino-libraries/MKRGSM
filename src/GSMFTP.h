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

#ifndef _GSM_FTP_H_INCLUDED
#define _GSM_FTP_H_INCLUDED

#include <Modem.h>

class GSMFTPElem;

class GSMFTP : public ModemUrcHandler {

public:
  GSMFTP();
  virtual ~GSMFTP();

  /** Connect to a FTP server
    @param hostname				FTP server hostname
    @param user						FTP user name
    @param password				FTP password
    @param port						FTP server port
    @param passiveMode		true if passive mode is active
    @return								true if no error
  */
  bool connect(String hostname, String user, String password, uint16_t port, bool passiveMode = true);
  /** Disconnect to the FTP server
    @return								true if no error
  */
  bool disconnect();
  /** Get informations of remote directory
    @param file		    class that contains the information of all the files found
    @param show							if true, display information of files
    @param timeout					maximum time allow to execute the function
    @return						true if no error
 */
  bool ls(GSMFTPElem& file, bool show = false, uint32_t timeout = 10000);
  /** Get informations of remote directory/file
    @param file							class that contains the information of all the files found
    @param name							name of file or directory where to search information
    @param file							name of file or directory where to search information
    @param show							if true, display information of files
    @param timeout					maximum time allow to execute the function
    @return						true if no error
 */
  bool ls(GSMFTPElem& file, const String name, bool show = false, uint32_t timeout = 10000);
  /** Create directory on the FTP server
    @param name							name of the directory to create
    @param timeout					maximum time allow to execute the function
    @return									true if no error
  */
  bool mkdir(const String& name, uint32_t timeout = 10000);
  /** Delete directory on the FTP server
  @param name							name of the directory to delete
  @param timeout					maximum time allow to execute the function
  @return									true if no error
  */
  bool removeDirectory(const String&, uint32_t timeout = 10000);
  /** Delete file on the FTP server
  @param name							name of the file to delete
  @param timeout					maximum time allow to execute the function
  @return									true if no error
  */
  bool removeFile(const String&, uint32_t timeout = 10000);
  /** Rename file on the FTP server
    @param oldName					name of the file to rename
    @param name							new name of the file to rename
    @param timeout					maximum time allow to execute the function
    @return									true if no error
  */
  bool rename(const String& oldName, const String& name, uint32_t timeout = 10000);
  /** Change of the working directory on the FTP server
    @param path							new working directory to move on
    @param timeout					maximum time allow to execute the function
    @return									true if no error
  */
  bool cd(const String& path, uint32_t timeout = 10000);
  /** Download a file from the FTP server
    @param localFileName		name of the file on filesystem sent from FTP server
    @param remoteFileName		name of file on FTP server to retreive on filesystem
    @param timeout					maximum time allow to execute the function, -1 infinite
    @return									true if no error

    Download a file in blocking mode until the timeout elapsed.
  */
  bool download(const String& localFileName, const String& remoteFileName, int32_t timeout = -1);
  /** Start file download from the FTP server
    @param localFileName		name of the file on filesystem sent from FTP server
    @param remoteFileName		name of file on FTP server to retreive on filesystem
    @return									true if no error

    Initialize the file download in non blocking mode.
  */
  bool downloadStart(const String& localFileName, const String& remoteFileName);
  /** Update download state
    @param remoteFileName		name of file on FTP server to retreive on filesystem
    @param localFileName		name of the file on filesystem sent from FTP server
    @param showProgression	if true show the downloading progression [%]
    @return									1 : download finished, 0 downloading, -1 an error occured

    Update the download in non blocking mode.
  */
  int downloadReady(const String& localFileName, bool showProgression);
  /** Upload a file to the FTP server
    @param localFileName		name of the file on filesystem to send to FTP server
    @param timeout					maximum time allow to execute the function, -1 infinite
    @return									true if no error
  */
  bool upload(const String& localFileName, const String& remoteFileName, int32_t timeout = -1);
  /** Start  file upload to the FTP server
  @param localFileName		name of the file on filesystem to send to FTP server
  @param remoteFileName		name of the file on FTP server sent from filesystem
  @return									true if no error

    Initialize the file upload in non blocking mode.
  */
  bool uploadStart(const String& localFileName, const String& remoteFileName);
  /** Update download state
  @return									1 : upload finished, 0 downloading, -1 an error occured

  Update the upload in non blocking mode.
  When uploading no other request can be send to the server, so we can not display the progress
  */
  int uploadReady();
  /** Direct upload from local volatile memory to FTP server
  @param data		          data to send
  @param size         		data size to send
  @param remoteFileName		name of the file on FTP server
  @param timeout		      maximum time before function return an error, -1 infinite
  @return									true if no error
  */
  bool write(void* data, size_t size, const String& remoteFileName, int32_t timeout = -1);
  /** Direct download from FTP server to local volatile memory
  @param data		          data to received
  @param size         		data size to received
  @param remoteFileName		name of the file on FTP server
  @param timeout		      maximum time before function return an error, -1 infinite
  @return									true if no error

  If size is less than the remote file size, the data reception is aborted before the complete downloaded.
  In this case the connection can be lost.
  */
  bool read(void* data, size_t size, const String& remoteFileName, int32_t timeout = -1);
  /** Start upload in stream mode
  @param remoteFileName		name of the file on FTP server
  @return									true if no error

  Set the module in direct link mode.
  After that it will establish a transparent end to end communication
  with the data connection TCP socket via the serial interface.
  No command to the module can be executed until the end of the transfer.
  */
  bool streamOutStart(const String& remoteFileName);
  /** Send data to FTP server
  @param data		          data to send
  @param size         		data size to send
  @return									true if no error

  Send a packet of data to the FTP server.
  */
  bool streamOut(void* data, size_t size);
  /** Finished the data transfer to FTP server
  @return									1 : transfer finished, 0 busy, -1 an error occured

  Exit direct link mode then wait for the transmission to be completed.
  */
  int streamOutReady();
  /** Start download in stream mode
  @param remoteFileName		name of the file on FTP server
  @return									true if no error

  Set the module in direct link mode.
  After that it will establish a transparent end to end communication
  with the data connection TCP socket via the serial interface.
  No command to the module can be executed until the end of the transfer.
  */
  bool streamInStart(const String& remoteFileName);
  /** Send data to FTP server
  @param data		          data to receive
  @param size         		data size to receive
  @param timeout		      maximum time before function return an error, -1 infinite
  @return									1 : all data is received, 0 not all data is received, -1 an error occured

  Send a packet of data to the FTP server.
  If all data is received the module will automatically exit from direct link mode.
  */
  int streamIn(void* data, size_t size, int32_t timeout = -1);
  /** Finished the data transfer to FTP server
  @return									1 : transfer finished, 0 busy, -1 an error occured

  Exit direct link mode then wait for the transmission to be completed.
  */
  int streamInReady();
  /** Print the error class and code of the last FTP operation
  @brief
  0,0 mean no error otherwise {error class},{error code}.
  For the description refer to the documention :
  https://www.u-blox.com/sites/default/files/u-blox-CEL_ATCommands_%28UBX-13002752%29.pdf
  */
  void printError();

private:
  static const uint32_t c_connectionTimeout = 10000;

  void handleUrc(const String&);

  int _connected;
  int _dirCreated;
  int _dirChanged;
  int _fileRemoved;
  int _dirRemoved;
  int _fileRenamed;
  int _fileDownloaded;
  int _fileUploaded;
  int _fileDirectUploaded;
  int _fileDirectDownloaded;
  GSMFTPElem* _fileInfo;
  uint32_t _downloadDisplayTimeRef;
  uint32_t _downloadRemoteFileSize;
  uint32_t _uploadRemainingBytes;
};

class GSMFTPElem
{
public:
  struct Elem
  {
    String permissions;
    uint32_t size;
    uint32_t number;
    String user;
    String group;
    String lastModified;
    String name;

    Elem() :size(0), number(0)
    {}
  };

  GSMFTPElem() :_elem(nullptr), _count(0) {}
  ~GSMFTPElem() { clear(); }

  /** Append a new element in the file array
  @param elem					elem to append in the array
  */
  void append(const Elem elem);
  /** Show file information of the corresponding index
  @param i						index of the element to show
  */
  void show(int i);
  /** Clear the file array
  */
  void clear();
  /** Get a file element
  @param i						file index to get the element
  @return							array element relative to the index, empty element if the index is out of range
  */
  Elem elem(uint16_t i);
  /** Get number of file found
  @return							number of element in the array
  */
  uint32_t count() { return _count; }
  /** Parse string containing file information
  @param str					string containing file information to parse
  */
  void parse(const String& str);

private:
  Elem* _elem;
  uint32_t _count;
};

#endif
