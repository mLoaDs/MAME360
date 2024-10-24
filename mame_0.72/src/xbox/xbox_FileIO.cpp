/**
	* \file			xbox_FileIO.cpp
	* \brief		Implementation of the "File I/O" section of
	*           osdepend.h
	*/

//= I N C L U D E S ====================================================
#include "osdepend.h"
#include "osd_cpu.h"

#include "DebugLogger.h"

#include "fileio.h"
#include "xbox_FileIO.h"
#include "StdString.h"
 
#include <set>
#include <crtdbg.h>
#include <stdio.h>

//= D E F I N E S ======================================================

  //!<  Helper macro, calls Helper_CreateOrOpenSystemPath on the passed path
#define CREATEOROPENPATH( _pth__, _wtReq__ )  tempStr = strdup( _pth__ ); \
                                              if( !Helper_CreateOrOpenSystemPath( &tempStr, (_wtReq__) ) ) { \
                                                PRINTMSG(( T_ERROR, "Failed to create or open system path %s!", tempStr )); \
                                              }

#define FILE_BUFFER_SIZE	256

//= S T R U C T U R E S ================================================

struct _osd_file
{
  HANDLE		m_handle;
  UINT64		m_filepos;
  UINT64		m_end;
  UINT64		m_offset;
  UINT64		m_bufferbase;
  DWORD		  m_bufferbytes;
  UINT8		  m_buffer[FILE_BUFFER_SIZE];

  UINT64    m_writeBufferBytes;
  UINT8     m_writeBuffer[FILE_BUFFER_SIZE];

} _osd_file;


//= G L O B A L = V A R S ==============================================
const char            *g_ROMBackupPath = NULL;
FileIOConfig_t        g_FileIOConfig;
const char            *g_pathNames[FILETYPE_MAMEOX_END][4] = {NULL};
std::set<CStdString>  g_setcSMBFilesCached;


//= P R O T O T Y P E S ================================================
  //--------------------------------------------------------------------
  //  Helper_CreateOrOpenSystemPath
  //! Create one of the system directories (GENERALPATH, etc...) or
  //!  open it if it already exists. Also, if the path cannot be created
  //!  on the D:, do so on the ALTDRIVE instead
  //!
  //! \param  path - [IN,OUT] Passed in as the path to create/verify,
  //!                this value is set to the actual path on return.
  //! \param  writeRequired - When set to TRUE, the path cannot be a
  //!                         read-only directory or file.
  //--------------------------------------------------------------------
static BOOL Helper_CreateOrOpenSystemPath( char **path, BOOL writeRequired );

  //--------------------------------------------------------------------
  // Helper_CreateDirectoryPath
  //! \brief  Ensures that the passed path is reachable, creating 
  //!         directories when necessary.
  //!
  //! \param  path - The path to check, note that this will be modified
  //!                in the function, but set back to the original on
  //!                return
  //! \param  has_filename - Whether or not "path" contains the name of
  //!                        a file.
  //--------------------------------------------------------------------
void Helper_CreateDirectoryPath( char *path, BOOL has_filename );
static void Helper_ConvertSlashToBackslash( char *path );

//= F U N C T I O N S ==================================================


//---------------------------------------------------------------------
//	InitializeFileIO
//---------------------------------------------------------------------
void InitializeFileIO( void )
{
  char *tempStr = NULL;
	PRINTMSG(( T_TRACE, "InitializeFileIO" ));
 

    // Try to create/open all of the paths. If any fail,
    //     switch the drive letter to ALTDRIVE, as we must be
    //     running off of a DVD
  CREATEOROPENPATH( g_FileIOConfig.m_romBackupPath.c_str(), TRUE );
  g_ROMBackupPath = tempStr;

  CREATEOROPENPATH( g_FileIOConfig.m_romPath0.c_str(), FALSE );
	g_pathNames[FILETYPE_RAW][0] = g_pathNames[FILETYPE_ROM][0] = tempStr;
  CREATEOROPENPATH( g_FileIOConfig.m_romPath1.c_str(), FALSE );
	g_pathNames[FILETYPE_RAW][1] = g_pathNames[FILETYPE_ROM][1] = tempStr;
  CREATEOROPENPATH( g_FileIOConfig.m_romPath2.c_str(), FALSE );
	g_pathNames[FILETYPE_RAW][2] = g_pathNames[FILETYPE_ROM][2] = tempStr;
  CREATEOROPENPATH( g_FileIOConfig.m_romPath3.c_str(), FALSE );
	g_pathNames[FILETYPE_RAW][3] = g_pathNames[FILETYPE_ROM][3] = tempStr;

  CREATEOROPENPATH( g_FileIOConfig.m_iniPath.c_str(), TRUE );
	g_pathNames[FILETYPE_INI][0] = tempStr;

  CREATEOROPENPATH( g_FileIOConfig.m_HiScorePath.c_str(), TRUE );
	g_pathNames[FILETYPE_HIGHSCORE][0] = g_pathNames[FILETYPE_HIGHSCORE_DB][0] = 
                                    tempStr;

  CREATEOROPENPATH( g_FileIOConfig.m_NVramPath.c_str(), TRUE );
	g_pathNames[FILETYPE_NVRAM][0] = g_pathNames[FILETYPE_STATE][0] =
	                                 g_pathNames[FILETYPE_MEMCARD][0] = tempStr;

  CREATEOROPENPATH( g_FileIOConfig.m_HDImagePath.c_str(), FALSE );
  g_pathNames[FILETYPE_IMAGE][0] = tempStr;

  CREATEOROPENPATH( g_FileIOConfig.m_HDImagePath.c_str(), TRUE );
  g_pathNames[FILETYPE_IMAGE_DIFF][0] = tempStr;

  CREATEOROPENPATH( g_FileIOConfig.m_artPath.c_str(), FALSE );
  g_pathNames[FILETYPE_ARTWORK][0] = tempStr;

  CREATEOROPENPATH( g_FileIOConfig.m_audioPath.c_str(), FALSE );
  g_pathNames[FILETYPE_SAMPLE][0] = tempStr;

  CREATEOROPENPATH( g_FileIOConfig.m_configPath.c_str(), TRUE );
  g_pathNames[FILETYPE_CONFIG][0] = tempStr;

  CREATEOROPENPATH( g_FileIOConfig.m_generalPath.c_str(), TRUE );
	g_pathNames[FILETYPE_INPUTLOG][0] = g_pathNames[FILETYPE_HISTORY][0] = 
																	    g_pathNames[FILETYPE_CHEAT][0] = 
																	    g_pathNames[FILETYPE_LANGUAGE][0] = 
																	    g_pathNames[FILETYPE_CTRLR][0] = tempStr;

  CREATEOROPENPATH( g_FileIOConfig.m_screenshotPath.c_str(), TRUE );
  g_pathNames[FILETYPE_SCREENSHOT][0] = tempStr;

    // The MAME core will look for autoboot save states as FILETYPE_STATE
    // so add a second path for save states.
  CREATEOROPENPATH( g_FileIOConfig.m_autoBootSavePath.c_str(), TRUE );
  g_pathNames[FILETYPE_STATE][1] = tempStr;

  CREATEOROPENPATH( DEFAULT_MAMEOXSYSTEMPATH, TRUE );
  g_pathNames[FILETYPE_MAMEOX_SYSTEM][0] = tempStr;

}

//---------------------------------------------------------------------
//	osd_get_path_count
//---------------------------------------------------------------------
int osd_get_path_count( int pathtype )
{
	/* Return the number of paths for a given type */
	PRINTMSG(( T_TRACE, "osd_get_path_count: pathtype: %d", pathtype ));

	switch( pathtype )
	{
    // ROMs are allowed to be in multiple directories
    // MAMEoX currently supports 4 ROM paths
	case FILETYPE_ROM:
    return 4;

	case FILETYPE_RAW:
	case FILETYPE_IMAGE:
	case FILETYPE_IMAGE_DIFF:
	case FILETYPE_SAMPLE:
	case FILETYPE_ARTWORK:

	case FILETYPE_NVRAM:
	case FILETYPE_HIGHSCORE:
	case FILETYPE_HIGHSCORE_DB:
	case FILETYPE_CONFIG:
	case FILETYPE_INPUTLOG:
	case FILETYPE_MEMCARD:
	case FILETYPE_SCREENSHOT:
	case FILETYPE_HISTORY:
	case FILETYPE_CHEAT:
	case FILETYPE_LANGUAGE:
	case FILETYPE_CTRLR:
	case FILETYPE_INI:
		return 1;

    // Save states are in the standard NVRAM dir,
    // as well as the autobootstate dir
	case FILETYPE_STATE:
    return 2;

    // Internal MAMEoX system files
  case FILETYPE_MAMEOX_SYSTEM:
    return 1;

    // The full path is given as the filename
  case FILETYPE_MAMEOX_FULLPATH:
    return 0;
	}

	return 0;
}

//---------------------------------------------------------------------
//	osd_get_path_info
//---------------------------------------------------------------------
int osd_get_path_info( int pathtype, int pathindex, const char *filename )
{
	/* Get information on the existence of a file */
	DWORD attribs;
	
	PRINTMSG(( T_TRACE, "osd_get_path_info, pathtype %d, pathindex %d", pathtype, pathindex ));

  #ifdef ALL_ROMS_ZIPPED
      // Don't bother searching for anything in the ROMs directory
      // if it doesn't end with .zip
    if( pathtype == FILETYPE_ROM )
    {
      if( stricmp( &filename[strlen(filename)-4], ".zip" ) )
        return PATH_NOT_FOUND;
    }
  #endif


  // Check if the path points to a SMB share, we only support files
  // on smb shares so always return this is a file

  CStdString strPath;
  if( pathtype != FILETYPE_MAMEOX_FULLPATH )
    strPath = g_pathNames[pathtype][pathindex];
  else
    strPath = filename;

  strPath.MakeLower();
  if( strPath.Left(6) == "smb:\\\\" || strPath.Left(6) == "smb://" )
    attribs = PATH_IS_FILE;
  else
  {
    if( pathtype != FILETYPE_MAMEOX_FULLPATH )
    {
      strPath += "\\";
      strPath += filename;
    }
    Helper_ConvertSlashToBackslash( strPath.GetBuffer(strPath.GetLength()+10) );
    attribs = GetFileAttributes( strPath.c_str() );
  }

	if( attribs == 0xFFFFFFFF )
		return PATH_NOT_FOUND;
	else if( attribs & FILE_ATTRIBUTE_DIRECTORY )
		return PATH_IS_DIRECTORY;

	return PATH_IS_FILE;
}









//---------------------------------------------------------------------
//	osd_fopen
//---------------------------------------------------------------------
osd_file *osd_fopen( int pathtype, int pathindex, const char *filename, const char *mode )
{
	/* 
			Attempt to open a file with the given name and 
			mode using the specified path type 
	*/

	DWORD dwCreationDisposition = 0;
	DWORD dwDesiredAccess = 0;	
	osd_file *ret;
  LARGE_INTEGER fileSize;

	PRINTMSG(( T_TRACE, "osd_fopen" ));

    // The calloc is important here, if it's taken away
    // it will cause odd behavior if the caller doesn't do
    // a seek to set the file pointer (it's otherwise uninitialized)
	ret = (osd_file*)calloc( 1, sizeof(osd_file) ); 
	if( !ret )
	{
		PRINTMSG(( T_ERROR, "Malloc failed creating osd_file instance!" ));
		return NULL;
	}

	if( strchr( mode, 'r' ) )
	{
		dwCreationDisposition = OPEN_EXISTING;
		dwDesiredAccess = GENERIC_READ;
	}
	if( strchr( mode, 'w' ) )
	{
		dwCreationDisposition = CREATE_ALWAYS;
		dwDesiredAccess = GENERIC_WRITE;
	}
	if (strchr(mode, '+'))
	{
		dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
	}

  CStdString strPath;
  if( pathtype != FILETYPE_MAMEOX_FULLPATH )
    strPath = g_pathNames[pathtype][pathindex];
  else
    strPath = filename;

  strPath.MakeLower();
  CStdString strFullPath = "";

 
  if( pathtype != FILETYPE_MAMEOX_FULLPATH )
    strFullPath.Format( "%s\\%s", g_pathNames[pathtype][pathindex], filename );
  else
    strFullPath = filename;

  // Only load the file from disk if it isn't a smb file
  //if ( ret->m_bIsSMB == FALSE )
  //{
    Helper_ConvertSlashToBackslash( strFullPath.GetBuffer(strFullPath.GetLength()+10) );

    PRINTMSG(( T_INFO, "Opening file: %s", strFullPath.c_str() ));

    ret->m_handle = CreateFile(	strFullPath.c_str(),
      dwDesiredAccess,
      0,
      NULL,
      dwCreationDisposition,
      FILE_ATTRIBUTE_NORMAL,
      NULL );

    if( ret->m_handle == INVALID_HANDLE_VALUE )
    {
      // See why it failed //
      DWORD err = GetLastError();

      // Return if read only or some other error
      if( !(dwDesiredAccess & GENERIC_WRITE) || err != ERROR_PATH_NOT_FOUND )
      {
        PRINTMSG(( T_ERROR, "Failed opening file %s: 0x%X!", strFullPath.c_str(), err ));
        free( ret );
        return NULL;
      }

      // create the directory path and try again
      Helper_CreateDirectoryPath( strFullPath.c_str(), TRUE );
      ret->m_handle = CreateFile(	strFullPath.c_str(),
                                  dwDesiredAccess,
                                  0,
                                  NULL,
                                  dwCreationDisposition,
                                  FILE_ATTRIBUTE_NORMAL,
                                  NULL );

      // if that doesn't work, we give up
      if( ret->m_handle == INVALID_HANDLE_VALUE )
      {
        PRINTMSG(( T_ERROR, "Failed opening file %s: 0x%X!", strFullPath.c_str(), err ));
        free( ret );
        return NULL;
      }
    }

    // Store the filesize
    if( !GetFileSizeEx( ret->m_handle, &fileSize ) )
    {
      PRINTMSG(( T_ERROR, "GetFileSizeEx failed!" ));
      return NULL;
    }

    ret->m_end = fileSize.QuadPart;
  //}

	return ret;
}


//---------------------------------------------------------------------
//  osd_fflush
//---------------------------------------------------------------------
void osd_fflush( osd_file *file )
{
  if( !file )
    return;

    // Flush the write buffer
  if( file->m_writeBufferBytes )
  {
    DWORD result;
    if( !WriteFile( file->m_handle, file->m_writeBuffer, file->m_writeBufferBytes, &result, NULL ) )
    {
      PRINTMSG(( T_ERROR, "WriteFile failed! 0x%X", GetLastError() ));
      return;
    }
    file->m_writeBufferBytes = 0;
    file->m_filepos += result;
  }
}


//---------------------------------------------------------------------
//	osd_fseek
//---------------------------------------------------------------------
INT32 osd_fseek( osd_file *file, INT64 offset, int whence )
{
	  // Seek within a file
  if( !file )
    return 1;

  
    osd_fflush( file );

    switch( whence )
    {
    case SEEK_SET:	
      file->m_offset = offset;
      break;

    case SEEK_CUR:	
      file->m_offset += offset;
      break;

    case SEEK_END:
      file->m_offset = file->m_end + offset;
      break;

    default:
      PRINTMSG(( T_ERROR, "Invalid whence parameter in osd_fseek" ));
      return 1;
    }

      // Validate arg
    if( file->m_offset > file->m_end )
    {
      PRINTMSG(( T_ERROR, "Offset value too high or low in osd_fseek" ));
      return 1;
    }

      // attempt to seek to the current location if we're not there already
    if( file->m_offset != file->m_filepos )
    {
      LONG upperPos = (LONG)(file->m_offset >> 32);
      DWORD result = SetFilePointer( file->m_handle, (UINT32)file->m_offset, &upperPos, FILE_BEGIN );
      if (result == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR)
      {
        file->m_filepos = ~0;
        return 1; //length - bytes_left;
      }
      file->m_filepos = file->m_offset;
      file->m_bufferbytes = 0;
    }  
   

	return 0;
}

//---------------------------------------------------------------------
//	osd_ftell
//---------------------------------------------------------------------
UINT64 osd_ftell( osd_file *file )
{
 
    return file->m_offset;
  
}

//---------------------------------------------------------------------
//	osd_feof
//---------------------------------------------------------------------
int osd_feof( osd_file *file )
{
  
    /* Return 1 if we're at the end of file */
    return (file->m_offset >= file->m_end);
   
}

//---------------------------------------------------------------------
//	osd_fread
//---------------------------------------------------------------------
UINT32 osd_fread( osd_file *file, void *buffer, UINT32 length )
{
  UINT32 bytes_left = length;
  UINT32 bytes_to_copy;
  DWORD result;

  osd_fflush( file );

  
    // handle data from within the buffer
    if (file->m_offset >= file->m_bufferbase && file->m_offset < file->m_bufferbase + file->m_bufferbytes )
    {
      // copy as much as we can
      bytes_to_copy = (int)(file->m_bufferbase + file->m_bufferbytes - file->m_offset);
      if (bytes_to_copy > length)
        bytes_to_copy = length;
      memcpy(buffer, &file->m_buffer[file->m_offset - file->m_bufferbase], bytes_to_copy);

      // account for it
      bytes_left -= bytes_to_copy;
      file->m_offset += bytes_to_copy;
      buffer = (UINT8 *)buffer + bytes_to_copy;

      // if that's it, we're done
      if (bytes_left == 0)
        return length;
    }

    // if we have a small read remaining, do it to the buffer and copy out the results
    if (length < (FILE_BUFFER_SIZE >> 1))
    {
      // read as much of the buffer as we can
      file->m_bufferbase = file->m_offset;
      file->m_bufferbytes = 0;
      ReadFile(file->m_handle, file->m_buffer, (DWORD)FILE_BUFFER_SIZE, &file->m_bufferbytes, NULL);
      file->m_filepos += file->m_bufferbytes;

      // copy it out
      bytes_to_copy = bytes_left;
      if (bytes_to_copy > file->m_bufferbytes)
        bytes_to_copy = file->m_bufferbytes;
      memcpy(buffer, file->m_buffer, bytes_to_copy);

      // adjust pointers and return
      file->m_offset += bytes_to_copy;
      bytes_left -= bytes_to_copy;
      return length - bytes_left;
    }

    // otherwise, just read directly to the buffer
    else
    {
      // do the read
      ReadFile(file->m_handle, buffer, bytes_left, &result, NULL);
      file->m_filepos += result;

      // adjust the pointers and return
      file->m_offset += result;
      bytes_left -= result;
      return length - bytes_left;
    }
  
}

//---------------------------------------------------------------------
//	osd_fwrite
//---------------------------------------------------------------------
UINT32 osd_fwrite( osd_file *file, const void *buffer, UINT32 length )
{
  DWORD result;

  // invalidate any buffered read data
  file->m_bufferbytes = 0;


  /*
    // attempt to seek to the current location if we're not there already
  if( file->m_offset != file->m_filepos )
  {
    LONG upperPos = (LONG)(file->m_offset >> 32);
    result = SetFilePointer(file->m_handle, (UINT32)file->m_offset, &upperPos, FILE_BEGIN);
    if (result == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR)
    {
      file->m_filepos = ~0;
      return 0;
    }
    file->m_filepos = file->m_offset;
  }
  */

    // Just copy the data into the write buffer
  if( (FILE_BUFFER_SIZE - file->m_writeBufferBytes) > length )
  {
    memcpy( &file->m_writeBuffer[file->m_writeBufferBytes], buffer, length );
    file->m_writeBufferBytes += length;
    result = length;
  }
  else
  {
      // Flush the buffer and then write the data
    osd_fflush( file );

      // Write the requested data to the file as well
    if( !WriteFile( file->m_handle, buffer, length, &result, NULL ) )
    {
      PRINTMSG(( T_ERROR, "WriteFile failed! 0x%X", GetLastError() ));
      return 0;
    }
    file->m_filepos += result;
  }

    // adjust the pointers
  file->m_offset += length;
  if( file->m_offset > file->m_end )
    file->m_end = file->m_offset;

  return result;
}

//---------------------------------------------------------------------
//	osd_fclose
//---------------------------------------------------------------------
void osd_fclose( osd_file *file )
{
  
	osd_fflush( file );

	  // close the handle and clear it out
	if (file->m_handle)
	  CloseHandle(file->m_handle);

	file->m_handle = NULL;
   

    // Free the file
  free( file );
}

//---------------------------------------------------------------------
//	Helper_CreateDirectoryPath
//---------------------------------------------------------------------
void Helper_CreateDirectoryPath( const char *path, BOOL has_filename )
{
	DWORD attributes;
	char *sep;
	PRINTMSG(( T_TRACE, "Helper_CreateDirectoryPath" ));
	
	if( !path )
		return;

	sep = (char *)strrchr( path, '\\' );

		// if there's still a separator, and it's not the root, nuke it and recurse
	if( sep && sep > path && sep[-1] != '\\' )
	{
		*sep = 0;
		Helper_CreateDirectoryPath( path, FALSE );
		*sep = '\\';
	}

		// if we have a filename, we're done
	if( has_filename )
		return;

		// if the path already exists, we're done
	attributes = GetFileAttributes(path);
	if( attributes >= 0 )
		return;

		// create the path
	CreateDirectory( path, NULL );
}

//---------------------------------------------------------------------
//	ComposeFilePath
//---------------------------------------------------------------------
BOOL ComposeFilePath( char *buf, UINT32 maxLen, UINT32 pathtype, UINT32 pathindex, const char *filename )
{
  if( !buf || !filename )
    return FALSE;

  if( pathtype != FILETYPE_MAMEOX_FULLPATH )
  {
    if( maxLen < strlen(g_pathNames[pathtype][pathindex]) + strlen(filename) + 2 )
      return FALSE;
    sprintf( buf, "%s\\%s", g_pathNames[pathtype][pathindex], filename );
  }
  else
  {
    if( maxLen < strlen(filename) + 1 )
      return FALSE;
    strcpy( buf, filename );
  }

	Helper_ConvertSlashToBackslash( buf );

  return TRUE;
}

//---------------------------------------------------------------------
//	Helper_ConvertSlashToBackslash
//---------------------------------------------------------------------
static void Helper_ConvertSlashToBackslash( char *path )
{
	char *slash;

	if( !path )
		return;

	slash = strrchr( path, '/' );
	while( slash )
	{
		*slash = '\\';
		slash = strrchr( slash, '/' );
	}
}

//---------------------------------------------------------------------
//  Helper_CreateOrOpenSystemPath
//---------------------------------------------------------------------
static BOOL Helper_CreateOrOpenSystemPath( char **path, BOOL writeRequired )
{
  DWORD attrib;
  if( !path || !(*path) )
    return FALSE;

  // If the path starts w/ smb:\\ assume it exists
  CStdString strPath = *path;
  if( strPath.Left(6) == "smb:\\\\" || strPath.Left(6) == "smb://" )
  {
    return TRUE;
  }

  attrib = GetFileAttributes( *path );
	if( attrib == PATH_NOT_FOUND || attrib == 0xFFFFFFFF || (writeRequired && (attrib & FILE_ATTRIBUTE_READONLY)) )
  {
    if( !CreateDirectory( *path, NULL ) )
    {
        // Attempt to create the directory on the ALTDRIVE instead
      *path[0] = g_FileIOConfig.m_ALTDrive[0];
      attrib = GetFileAttributes( *path );
	    if( attrib == PATH_NOT_FOUND || attrib == 0xFFFFFFFF || (writeRequired && (attrib & FILE_ATTRIBUTE_READONLY)) )
      {
        if( !CreateDirectory( *path, NULL ) )
          return FALSE;
      }
    }
  }

  return TRUE;
}
