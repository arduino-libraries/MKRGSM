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

class GSMFTP : public ModemUrcHandler {

public:
	struct FTPFileElem
	{
		String permissions;
		uint32_t size;
		uint32_t number;
		String user;
		String group;
		String lastModified;
		String name;

		FTPFileElem() :size(0), number(0)
		{}
	};

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
	bool connect(String hostname, String user, String password, uint16_t port, bool passiveMode=true);
	/** Disconnect to the FTP server
		@return								true if no error
	*/
	bool disconnect();
	/** Get informations of remote directory
		@param show							if true, display information of files
		@param timeout					maximum time allow to execute the function
		@return						true if no error
 */
	bool ls(bool show=false, uint32_t timeout=10000);
	/** Get file number of remote directory
		@return									number of file read after the call of @ref ls function
	*/
	uint32_t fileCount() { return _file.count; }
	/** Get a file element
		@param i								index of the file element array to retreive
		@return									file element
	*/
	FTPFileElem file(uint16_t i);
	/** Create directory on the FTP server
		@param name							name of the directory to create
		@param timeout					maximum time allow to execute the function
		@return									true if no error
	*/
	bool mkdir(const String& name, uint32_t timeout=10000);
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
		@param remoteFileName		name of file on FTP server to retreive on filesystem
		@param localFileName		name of the file on filesystem sent from FTP server
		@param timeout					maximum time allow to execute the function
		@return									true if no error
	*/
	bool download(const String& remoteFileName, const String& localFileName, uint32_t timeout = 10000);
	/** Upload a file to the FTP server
		@param localFileName		name of the file on filesystem to send to FTP server
		@param remoteFileName		name of the file on FTP server sent from filesystem
		@param timeout					maximum time allow to execute the function
		@return									true if no error
	*/
	bool upload(const String& localFileName, const String&remoteFileName, uint32_t timeout = 10000);

	/** Print the error class and code of the last FTP operation
		@brief 
		0,0 mean no error otherwise {error class},{error code}.
		For the description refer to the documention : 
		https://www.u-blox.com/sites/default/files/u-blox-CEL_ATCommands_%28UBX-13002752%29.pdf
	*/
	void printError();

private:
	static const uint32_t c_connectionTimeout = 10000;

	struct FTPFile
	{
		FTPFileElem* e;
		uint32_t count;
		bool ready;

		void clear();
		void append(const FTPFileElem&);
		void show(int);
		void parse(const String&);

		FTPFile() :e(nullptr), count(0), ready(true)
		{}
	};

	void handleUrc(const String&);

	bool _connected;
	int _dirCreated;
	int _dirChanged;
	int _fileRemoved;
	int _dirRemoved;
	int _fileRenamed;
	int _fileDownloaded;
	int _fileUploaded;
	FTPFile _file;
};

#endif
