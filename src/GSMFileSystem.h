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

#ifndef _GSM_FILE_SYSTEM_H_INCLUDED
#define _GSM_FILE_SYSTEM_H_INCLUDED

#include <Arduino.h>

class GSMFileSytem {

public:
	struct FileElem
	{
		String name;
		uint32_t size;

		FileElem() :size(0)
		{}
	};

  GSMFileSytem();
  virtual ~GSMFileSytem();
	/** Get name and size of all the file system files 
		@param show				if true, display name and size of the files
		@param timeout		maximum time allow to execute the function
		@return						true if no error
 */
	bool ls(bool show = false, uint32_t timeout = 10000);
	/** Delete a file of the file system
		@param name				name of the file to delete
		@return						true if no error
	*/
	bool remove(const String& name);
	/** Get file number of the file system
		@return						number of file read after the call of @ref ls function
	*/
	uint32_t fileCount() { return _file.count; }
	/** Get a file element
		@param i					index of the file element array to retreive
		@return						file element 
	*/
	FileElem file(uint16_t i);
	/** Get a free space of the file system in bytes
		@return						free space
	*/
	uint32_t freeSpace();
	/** Create a file with data in the filesystem
		@param name				name of the file to create
		@param data				address of the data to write
		@param size				size of the data to write
		@return						true if no error
	*/
	bool write(const String& name, void* data, size_t size);
	/** Read a file in the filesystem
		@param name				name of the file to read
		@param data				address of read data 
		@param size				size of the data to read
		@return						true if no error
	*/
	bool read(const String& name, void* data, size_t size);

private:
	struct File
	{
		FileElem* e;
		uint32_t count;

		void clear();
		void append(const FileElem&);
		void show(int);
		void parse(const String&);

		File() :e(nullptr), count(0)
		{}
	};

	File _file;
};

#endif
