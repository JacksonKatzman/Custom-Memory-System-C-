//-----------------------------------------------------------------------------
// Copyright Ed Keenan 2018
// Optimized C++
//----------------------------------------------------------------------------- 
// FILE_IO  v.2.1.0
//          v.2.8   - warning 5039 windows.h
//          v.2.9   - fence
//----------------------------------------------------------------------------- 

#ifndef FILE_IO_H
#define FILE_IO_H

class FileIO
{
public:
	static void Open(const char * const firstName, const char * const lastName);
	static void Close();
	static FILE *GetHandle();

private:
	// big four
	FileIO();
	~FileIO();
	FileIO(const FileIO &) = delete;
	FileIO & operator=(const FileIO &) = delete;

	// methods
	static FileIO	*privGetInstance();
	void			privClose();
	void			privOpen(const char * const firstName, const char * const lastName);
	FILE			*privGetHandle();

	// data
	FILE *fileHandle;

};

#endif 

// ---  End of File ---------------
