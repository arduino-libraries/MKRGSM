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

class GSMFileSystemElem;

class GSMFileSystem {

public:

	/** Get name and size of all the file system files 
		@param show				if true, display name and size of the files
		@param timeout		maximum time allow to execute the function
    @param file		    class that contains the information of all the files found
		@return						true if no error
 */
  bool ls(GSMFileSystemElem& file, bool show = false, uint32_t timeout = 10000);
	/** Delete a file of the file system
		@param name				name of the file to delete
		@return						true if no error
	*/
	bool remove(const String& name);
	/** Delete files of the file system
		@param files			set of files to delete
		@return						true if no error
	*/
	bool remove(GSMFileSystemElem& files);
	/** Get a free space of the file system in bytes
		@return						free space
	*/
	uint32_t freeSpace();
  /** Create a file with data in the filesystem
    @param name				name of the file 
    @return						size of the file, -1 if error
  */
  int32_t size(const String& name);
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

};

class GSMFileSystemElem {

public:
  struct Elem
  {
    String name;
    uint32_t size;

    Elem() :size(0)
    {}
  };

  GSMFileSystemElem() :_elem(nullptr), _count(0){}
  ~GSMFileSystemElem() { clear(); }
  /** Get file number of the file system
  @return						number of file read after the call of @ref ls function
*/
  inline uint32_t count() { return _count; }
  /** Get a file element
    @param i					file index to get the element
    @return						file element
  */
  Elem elem(uint16_t i);
	/** Get file size
		@param i					file index to get the size
		@return						file size
	*/
  void setSize(uint16_t i, uint32_t size);
	/** Clear the file array
	*/
  void clear();
	/** Append a new element in the file array
	@param elem					elem to append in the array
	*/
  void append(const Elem elem);
	/** Show file information of the corresponding index
	@param i						index of the element to show
	*/
  void show(int i);
	/** Parse string containing file information
	@param str					string containing file information to parse
	*/
  void parse(const String& str);

private:
  Elem* _elem;
  uint32_t _count;
};

extern GSMFileSystem FILESYSTEM;

#endif
