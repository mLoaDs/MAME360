/**
	* \file			xbox_FileIO.h
	* \brief		Declarations for the FileIO subsystem
	*/

#pragma once

//= I N C L U D E S ====================================================

#include "xbox_Mame.h"
#include "osdepend.h"
#include "osd_cpu.h"
#include "fileio.h"
#include "StdString.h"

//= D E F I N E S ======================================================

#define DEFAULT_ALTDRIVE            "GAME"

#define DEFAULT_GENERALPATH			"GAME:\\GENERAL"
#define DEFAULT_ARTPATH				"GAME:\\ARTWORK"
#define DEFAULT_INIPATH				"GAME:\\INI"
#define DEFAULT_NVRAMPATH			"GAME:\\NVRAM"
#define DEFAULT_CONFIGPATH          "GAME:\\CFG"
#define DEFAULT_ROMPATH				"GAME:\\ROMS"
#define DEFAULT_AUDIOPATH           "GAME:\\SAMPLES"
#define DEFAULT_ROMBACKUPPATH       "GAME:\\ROMS\\BACKUP"
#define DEFAULT_HDIMAGEPATH         "GAME:\\HDIMAGES"
#define DEFAULT_HISCOREPATH         "GAME:\\HISCORES"
#define DEFAULT_SCREENSHOTPATH      "GAME:\\SCREENSHOTS"
#define DEFAULT_SKINPATH			"GAME:\\SKINS"

#define DEFAULT_CMAPPING            "\\Device\\Harddisk0\\Partition1"
#define DEFAULT_EMAPPING            "\\Device\\Harddisk0\\Partition1"
#define DEFAULT_FMAPPING            "\\Device\\Harddisk0\\Partition6"
#define DEFAULT_GMAPPING            "\\Device\\Cdrom0"
#define DEFAULT_HMAPPING            "\\Device\\Harddisk0\\Partition6"

  // Path in which "auto boot" save states are stored.
#define DEFAULT_BOOTSAVESTATE       "GAME:\\AUTOBOOTSAVES"

  // Path for internal files (should always be on the T or U drive)
#define DEFAULT_MAMEOXSYSTEMPATH    "GAME:\\SYSTEM"

#define INIFILENAME           "MAME.ini"
#define DRIVERLISTFILENAME    "DRIVERS.list"
#define ROMLISTFILENAME		    "ROMS.list"
#define ROMSTATUSFILENAME     "ROMStatus.xml"
#define ROMMETADATAFILENAME   "ROMS.metadata"

  //!<  Define this to force all open() calls on the ROM directory
  //!<  to fail if they do not end in the letters "zip"
  //!<  This can drastically speed up the ROM loading process, as
  //!<  we don't waste any time searching for files that are definitely not there
#define ALL_ROMS_ZIPPED


  // Special filetype for 
#define FILETYPE_MAMEOX_FULLPATH      FILETYPE_end                  //!<  The entire filepath is passed, not just the filename
#define FILETYPE_MAMEOX_SYSTEM        FILETYPE_MAMEOX_FULLPATH + 1    //!<  Use the DEFAULT_MAMEOXSYSTEMPATH as the path
#define FILETYPE_MAMEOX_END           FILETYPE_MAMEOX_SYSTEM + 1

//= S T R U C T U R E S ===============================================
struct FileIOConfig_t
{
  CStdString	m_ALTDrive;
  CStdString	m_generalPath;
  CStdString	m_artPath;
  CStdString	m_iniPath;
  CStdString	m_NVramPath;
  CStdString	m_configPath;
  CStdString	m_romPath0;
  CStdString	m_romPath1;
  CStdString	m_romPath2;
  CStdString	m_romPath3;
  CStdString	m_audioPath;
  CStdString	m_romBackupPath;
  CStdString	m_HDImagePath;
  CStdString	m_HiScorePath;
  CStdString	m_screenshotPath;
  CStdString	m_autoBootSavePath;
	CStdString	m_skinPath;

  CStdString	m_shaderFileName;

  CStdString	m_letterCMapping;
  CStdString	m_letterEMapping;
  CStdString	m_letterFMapping;
  CStdString	m_letterGMapping;
  CStdString	m_letterHMapping;

  BOOL				m_recursiveSearching;    //!< Whether or not to recursively search ROM/other paths

  FileIOConfig_t( void ) {
    // Default the paths
    m_ALTDrive            = DEFAULT_ALTDRIVE;
    m_generalPath         = DEFAULT_GENERALPATH;
    m_artPath             = DEFAULT_ARTPATH;
    m_iniPath             = DEFAULT_INIPATH;
    m_NVramPath           = DEFAULT_NVRAMPATH;
    m_configPath          = DEFAULT_CONFIGPATH;
    m_romPath0            = DEFAULT_ROMPATH;
    m_romPath1            = DEFAULT_ROMPATH;
    m_romPath2            = DEFAULT_ROMPATH;
    m_romPath3            = DEFAULT_ROMPATH;
    m_audioPath           = DEFAULT_AUDIOPATH;
    m_romBackupPath       = DEFAULT_ROMBACKUPPATH;
    m_HDImagePath         = DEFAULT_HDIMAGEPATH;
    m_HiScorePath         = DEFAULT_HISCOREPATH;
    m_screenshotPath      = DEFAULT_SCREENSHOTPATH;
    m_autoBootSavePath    = DEFAULT_BOOTSAVESTATE;
		m_skinPath						= DEFAULT_SKINPATH;

    m_letterCMapping      = DEFAULT_CMAPPING;
    m_letterEMapping      = DEFAULT_EMAPPING;
    m_letterFMapping      = DEFAULT_FMAPPING;
    m_letterGMapping      = DEFAULT_GMAPPING;
    m_letterHMapping      = DEFAULT_HMAPPING;
  }

  void MakeLower( void ) {
    m_ALTDrive.MakeLower();
    m_generalPath.MakeLower();
    m_artPath.MakeLower();
    m_iniPath.MakeLower();
    m_NVramPath.MakeLower();
    m_configPath.MakeLower();
    m_romPath0.MakeLower();
    m_romPath1.MakeLower();
    m_romPath2.MakeLower();
    m_romPath3.MakeLower();
    m_audioPath.MakeLower();
    m_romBackupPath.MakeLower();
    m_HDImagePath.MakeLower();
    m_HiScorePath.MakeLower();
    m_screenshotPath.MakeLower();
    m_autoBootSavePath.MakeLower();
		m_skinPath.MakeLower();

    m_letterCMapping.MakeLower();
    m_letterEMapping.MakeLower();
    m_letterFMapping.MakeLower();
    m_letterGMapping.MakeLower();
    m_letterHMapping.MakeLower();
  }
};

//= G L O B A L = V A R S ==============================================
extern const char       *g_ROMBackupPath;
extern FileIOConfig_t   g_FileIOConfig;

//= P R O T O T Y P E S ================================================
	//--------------------------------------------------------------------------
	//	InitializeFileIO
	//! \brief		Initializes the FileIO 
	//!            subsystem
	//--------------------------------------------------------------------------
void InitializeFileIO( void );


	//--------------------------------------------------------------------------
	//	Helper_CreateDirectoryPath
	//! \brief		Create a directory path on the XBOX HDD
	//!
	//!	\param		path - The path to create (note that this is temporarily 
	//!                   modified by the function, but set back to the original
  //!                   value on exit.)
	//!	\param		hasFilename - Whether or not "path" includes the name of a file
	//--------------------------------------------------------------------------
void Helper_CreateDirectoryPath( const char *path, BOOL hasFilename );

	//--------------------------------------------------------------------------
	//	ComposeFilePath
	//! \brief		Returns the full file path for the given options
	//!
	//!	\param		buf - [OUT] The buffer where the file path should be stored
	//!	\param		maxLen - The maximum number of characters to store in buf
  //! \param    pathtype - The path type (see fileio.h)
  //! \param    pathindex - The path index (always set to 0)
  //! \param    filename - The filename to append to the path
  //!
  //! \return   BOOL - Operation status
  //! \retval   TRUE - Valid path returned
  //! \retval   FALSE - No path returned
	//--------------------------------------------------------------------------
BOOL ComposeFilePath( char *buf, UINT32 maxLen, UINT32 pathtype, UINT32 pathindex, const char *filename );


