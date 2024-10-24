/**
	* \file			MAMEoXUtil.cpp
	* \brief		MAMEoX utility functions
	*
	*/


//= I N C L U D E S ====================================================
#include "xbox_Mame.h"

#include "InputManager.h"
#include "GraphicsManager.h"
#include "DebugLogger.h"
#include "XBESectionUtil.h"

#include "FontSet.h"
 

#include "System_IniFile.h"
#include "xbox_Timing.h"
#include "xbox_JoystickMouse.h"
#include "xbox_Direct3DRenderer.h"
#include "xbox_FileIO.h"
 
extern "C" {
#include "osd_cpu.h"
#include "driver.h"
#include "mame.h"
 
}

//= D E F I N E S =====================================================
#define MAPDRIVE( _drivePath__, _driveLetter__ )    {}

//= S T R U C T U R E S ===============================================
typedef struct _UNICODE_STRING
{
  USHORT      m_length;
  USHORT      m_maxLength;
  const char *m_str;
} UNICODE_STRING, *PUNICODE_STRING;

//= G L O B A L = V A R S =============================================
CInputManager			    g_inputManager;
CGraphicsManager	    g_graphicsManager;
CFontSet              g_fontSet;          // The global font manager
 

extern BOOL           g_soundEnabled;   // Sound processing override (defined in xbox_Main.cpp)
extern char			  pshader[255]; 

 

ROMListOptions_t      g_romListOptions;
SkinOptions_t		  g_skinOptions;
MAMEoXLaunchData_t    g_persistentLaunchData;   //!<  Launch data that persists via the INI
/*
#pragma data_seg()
#pragma bss_seg()
*/

extern UINT32         g_screensaverTimeout;     //!<  Time before the Launcher screensaver kicks in

extern "C" int fatalerror( const char *fmt, ... );

extern "C" {

  // from MAME\cheat.c
extern char *cheatfile;
extern const char *history_filename;  // Defined in datafile.c

  //! Lightgun calibration data
lightgunCalibration_t    g_calibrationData[4] = { {-32767,0,32767,32767,0,-32767},
                                                  {-32767,0,32767,32767,0,-32767},
                                                  {-32767,0,32767,32767,0,-32767},
                                                  {-32767,0,32767,32767,0,-32767} };

  //! The fonttype to use for *FontRender
static fonttype          g_fontType = FONTTYPE_DEFAULT;

//= P R O T O T Y P E S ===============================================
XBOXAPI DWORD WINAPI IoCreateSymbolicLink( PUNICODE_STRING symLinkName, PUNICODE_STRING devName );
XBOXAPI DWORD WINAPI IoDeleteSymbolicLink( PUNICODE_STRING symLinkName );


//= F U N C T I O N S =================================================



//-------------------------------------------------------------
//  osd_vmm_malloc
//-------------------------------------------------------------
void *osd_vmm_malloc( size_t size )
{
 

  return NULL;
}

//-------------------------------------------------------------
//  osd_vmm_free
//-------------------------------------------------------------
void osd_vmm_free( void *ptr )
{
  
}

//-------------------------------------------------------------
//  osd_vmm_accessaddress
//-------------------------------------------------------------
BOOL osd_vmm_accessaddress( void *ptr )
{
  return TRUE;
}

//-------------------------------------------------------------
//  osd_vmm_unloadLRUpage
//-------------------------------------------------------------
BOOL osd_vmm_unloadLRUpage( void )
{
  return TRUE;
}

//-------------------------------------------------------------
//  osd_vmm_printinfo
//-------------------------------------------------------------
void osd_vmm_printinfo( void )
{
 
}

//-------------------------------------------------------------
//  vsnprintf
//-------------------------------------------------------------
int vsnprintf( char *buf, size_t count, const char *fmt, va_list lst )
{
  return vsprintf( buf, fmt, lst );
}


// Note: The "STARTUP" segment is unloaded in xbox_JoystickMouse.c
 

//-------------------------------------------------------------
//	LoadOptions
//-------------------------------------------------------------
void LoadOptions( void )
{
  CStdString iniFileName = DEFAULT_MAMEOXSYSTEMPATH "\\" INIFILENAME;
  CSystem_IniFile iniFile( iniFileName );

    // 1 to enable cheating
  options.cheat = iniFile.GetProfileInt( "General", "CheatsEnabled", FALSE );
  cheatfile = strdup( iniFile.GetProfileString( "General", "CheatFilename", "cheat.dat" ).c_str() );
  if( !cheatfile )
    options.cheat = FALSE;
  history_filename = NULL; //strdup( iniFile.GetProfileString( "General", "HistoryFilename", "history.dat" ).c_str() );  
  options.skip_disclaimer = iniFile.GetProfileInt( "General", "SkipDisclaimer", FALSE );   // 1 to skip the disclaimer screen at startup
	options.skip_gameinfo = iniFile.GetProfileInt( "General", "SkipGameInfo", FALSE );    // 1 to skip the game info screen at startup
  g_screensaverTimeout =  iniFile.GetProfileInt( "General", "ScreenSaverTimeout", 10 );    // Minutes before screensaver in Launcher kicks in

  g_soundEnabled = iniFile.GetProfileInt( "Sound", "SoundEnable", TRUE );
    // sound sample playback rate, in Hz
  options.samplerate = iniFile.GetProfileInt( "Sound", "SampleRate", 44100 );
    // 1 to enable external .wav samples
  options.use_samples = iniFile.GetProfileInt( "Sound", "UseSamples", TRUE );
    // 1 to enable FIR filter on final mixer output
  options.use_filter = iniFile.GetProfileInt( "Sound", "UseFilter", TRUE );

  g_rendererOptions.m_vsync =               iniFile.GetProfileInt( "Video", "VSYNC", FALSE );       // Enable VSYNC for game rendering
  g_rendererOptions.m_throttleFramerate =   iniFile.GetProfileInt( "Video", "ThrottleFramerate", TRUE ); // Sync only to vsync
  g_rendererOptions.m_preserveAspectRatio = iniFile.GetProfileInt( "Video", "AspectRatioCorrection", TRUE );  // aspect ratio correction code
  g_rendererOptions.m_screenRotation =      (screenrotation_t)iniFile.GetProfileInt( "Video", "ScreenRotation", SR_0 );
  g_rendererOptions.m_frameskip =           iniFile.GetProfileInt( "Video", "Frameskip", 0 );
  g_rendererOptions.m_minFilter =           (D3DTEXTUREFILTERTYPE)iniFile.GetProfileInt( "Video", "MinificationFilter", D3DTEXF_LINEAR );
  g_rendererOptions.m_magFilter =           (D3DTEXTUREFILTERTYPE)iniFile.GetProfileInt( "Video", "MagnificationFilter", D3DTEXF_LINEAR );
 
 
	options.brightness =    iniFile.GetProfileFloat( "Video", "Brightness", 1.0f );		    // brightness of the display
  options.pause_bright =  iniFile.GetProfileFloat( "Video", "PauseBrightness", 0.65f );     // brightness when in pause
	options.gamma =         iniFile.GetProfileFloat( "Video", "Gamma", 1.0f );			        // gamma correction of the display
	options.color_depth =   0;//iniFile.GetProfileInt( "Video", "ColorDepth", 15 );
	// int		ui_orientation;	        // orientation of the UI relative to the video
  
 

  FLOAT xPercentage = iniFile.GetProfileFloat( "Video", "ScreenUsage_X", DEFAULT_SCREEN_X_PERCENTAGE );
  FLOAT yPercentage = iniFile.GetProfileFloat( "Video", "ScreenUsage_Y", DEFAULT_SCREEN_Y_PERCENTAGE );
  SetScreenUsage( xPercentage, yPercentage );

  FLOAT xPosition = iniFile.GetProfileFloat( "Video", "ScreenPos_X", 0.0f );
  FLOAT yPosition = iniFile.GetProfileFloat( "Video", "ScreenPos_Y", 0.0f );
  SetScreenPosition( xPosition, yPosition );

  options.use_artwork = iniFile.GetProfileInt( "Video", "Artwork", ARTWORK_USE_BACKDROPS | ARTWORK_USE_OVERLAYS | ARTWORK_USE_BEZELS );
  options.artwork_res = 0;
  options.artwork_crop = FALSE;

	options.vector_width =  iniFile.GetProfileInt( "VectorOptions", "VectorWidth", 640 );	      // requested width for vector games; 0 means default (640)
	options.vector_height = iniFile.GetProfileInt( "VectorOptions", "VectorHeight", 480 );	    // requested height for vector games; 0 means default (480)

    //- Vector options ------------------------------------------------------------------------------------
	options.beam = iniFile.GetProfileInt( "VectorOptions", "BeamWidth", 2 );			            // vector beam width
	options.vector_flicker = iniFile.GetProfileFloat( "VectorOptions", "FlickerEffect", 0.5f );	  // vector beam flicker effect control
	options.vector_intensity = iniFile.GetProfileFloat( "VectorOptions", "BeamIntensity", 1.5f );  // vector beam intensity
	options.translucency = iniFile.GetProfileInt( "VectorOptions", "Translucency", TRUE );      // 1 to enable translucency on vectors
	 
    // Antialiasing holds forever in vector.c due to an apparent signed/unsigned problem
  options.antialias = FALSE; //iniFile.GetProfileInt( "VectorOptions", "Antialiasing", FALSE );		    // 1 to enable antialiasing on vectors

	//char	savegame;		            character representing a savegame to load



  // Grab the directory settings
  g_FileIOConfig.m_ALTDrive           = iniFile.GetProfileString( "Directories", "ALTDrive",            DEFAULT_ALTDRIVE );
  g_FileIOConfig.m_letterCMapping     = iniFile.GetProfileString( "Directories", "C_Mapping",           DEFAULT_CMAPPING );
  g_FileIOConfig.m_letterEMapping     = iniFile.GetProfileString( "Directories", "E_Mapping",           DEFAULT_EMAPPING );
  g_FileIOConfig.m_letterFMapping     = iniFile.GetProfileString( "Directories", "F_Mapping",           DEFAULT_FMAPPING );
  g_FileIOConfig.m_letterGMapping     = iniFile.GetProfileString( "Directories", "G_Mapping",           DEFAULT_GMAPPING );
  g_FileIOConfig.m_letterHMapping     = iniFile.GetProfileString( "Directories", "H_Mapping",           DEFAULT_HMAPPING );

  g_FileIOConfig.m_romBackupPath      = iniFile.GetProfileString( "Directories", "BackupPath",          DEFAULT_ROMBACKUPPATH );
  g_FileIOConfig.m_romPath0           = iniFile.GetProfileString( "Directories", "RomsPath0",           DEFAULT_ROMPATH);
  g_FileIOConfig.m_romPath1           = iniFile.GetProfileString( "Directories", "RomsPath1",           DEFAULT_ROMPATH);
  g_FileIOConfig.m_romPath2           = iniFile.GetProfileString( "Directories", "RomsPath2",           DEFAULT_ROMPATH);
  g_FileIOConfig.m_romPath3           = iniFile.GetProfileString( "Directories", "RomsPath3",           DEFAULT_ROMPATH);
  g_FileIOConfig.m_artPath            = iniFile.GetProfileString( "Directories", "ArtPath",             DEFAULT_ARTPATH );
  g_FileIOConfig.m_audioPath          = iniFile.GetProfileString( "Directories", "AudioPath",           DEFAULT_AUDIOPATH );
  g_FileIOConfig.m_configPath         = iniFile.GetProfileString( "Directories", "ConfigPath",          DEFAULT_CONFIGPATH );
  g_FileIOConfig.m_generalPath        = iniFile.GetProfileString( "Directories", "GeneralPath",         DEFAULT_GENERALPATH );
  g_FileIOConfig.m_HDImagePath        = iniFile.GetProfileString( "Directories", "HDImagePath",         DEFAULT_HDIMAGEPATH );
  g_FileIOConfig.m_HiScorePath        = iniFile.GetProfileString( "Directories", "HiScoresPath",        DEFAULT_HISCOREPATH );
  g_FileIOConfig.m_NVramPath          = iniFile.GetProfileString( "Directories", "NVRamPath",           DEFAULT_NVRAMPATH );
  g_FileIOConfig.m_screenshotPath     = iniFile.GetProfileString( "Directories", "ScreenshotPath",      DEFAULT_SCREENSHOTPATH );
  g_FileIOConfig.m_autoBootSavePath   = iniFile.GetProfileString( "Directories", "AutoBootSavePath",    DEFAULT_BOOTSAVESTATE );

  g_FileIOConfig.m_shaderFileName	  = iniFile.GetProfileString( "Directories", "CurrentShader", pshader );
  strcpy(pshader, g_FileIOConfig.m_shaderFileName.c_str());

  g_FileIOConfig.MakeLower();

  

    //-- ROM List Options --------------------------------------------------------------------
  g_romListOptions.m_displayMode = (ROMListDisplayMode)iniFile.GetProfileInt( "ROMListOptions", "DisplayMode", DM_VERBOSELIST );
  g_romListOptions.m_sortMode =       (ROMListSortMode)iniFile.GetProfileInt( "ROMListOptions", "SortMode", (UINT32)SM_BYNAME );
  g_romListOptions.m_showROMStatus =                   iniFile.GetProfileInt( "ROMListOptions", "ShowROMStatus", TRUE );
  g_romListOptions.m_hideFiltered =                    iniFile.GetProfileInt( "ROMListOptions", "HideFilteredROMs", FALSE );
  g_romListOptions.m_filterMode =                      iniFile.GetProfileInt( "ROMListOptions", "FilterMode", (UINT32)FM_CLONE );
//  g_romListOptions.m_numPlayersFilter =                iniFile.GetProfileInt( "ROMListOptions", "Filter_NumPlayers", 0 );
  g_persistentLaunchData.m_cursorPosition =            iniFile.GetProfileFloat( "ROMListOptions", "CursorPosition", 0.0f );
  g_persistentLaunchData.m_pageOffset =                iniFile.GetProfileFloat( "ROMListOptions", "PageOffset", 0.0f );
  g_persistentLaunchData.m_superscrollIndex =          iniFile.GetProfileInt(   "ROMListOptions", "SuperscrollIndex", 0 );


		//-- Skin Options ------------------------------------------------------------------------
	g_skinOptions.m_currentSkin = iniFile.GetProfileString( "SkinOptions", "SelectedSkin", "Original" );



}


//-------------------------------------------------------------
//	SaveOptions
//-------------------------------------------------------------
void SaveOptions( void )
{
  CStdString iniFileName = DEFAULT_MAMEOXSYSTEMPATH "\\" INIFILENAME;
  CSystem_IniFile iniFile( iniFileName );

  iniFile.WriteProfileInt( "General", "CheatsEnabled", options.cheat );
  if( cheatfile )
    iniFile.WriteProfileString( "General", "CheatFilename", cheatfile );
  if( history_filename )
    iniFile.WriteProfileString( "General", "HistoryFilename", history_filename );

  iniFile.WriteProfileInt( "General", "SkipDisclaimer", options.skip_disclaimer );
  iniFile.WriteProfileInt( "General", "SkipGameInfo", options.skip_gameinfo );
  iniFile.WriteProfileInt( "General", "ScreenSaverTimeout", g_screensaverTimeout );


  iniFile.WriteProfileInt( "Sound", "SoundEnable", g_soundEnabled );
  iniFile.WriteProfileInt( "Sound", "SampleRate", options.samplerate );
  iniFile.WriteProfileInt( "Sound", "UseSamples", options.use_samples );
  iniFile.WriteProfileInt( "Sound", "UseFilter", options.use_filter );


  iniFile.WriteProfileInt( "Video", "VSYNC", g_rendererOptions.m_vsync );       // Enable VSYNC for game rendering
  iniFile.WriteProfileInt( "Video", "ThrottleFramerate", g_rendererOptions.m_throttleFramerate ); // Sync only to vsync
  iniFile.WriteProfileInt( "Video", "AspectRatioCorrection", g_rendererOptions.m_preserveAspectRatio );
  iniFile.WriteProfileInt( "Video", "MinificationFilter", g_rendererOptions.m_minFilter );
  iniFile.WriteProfileInt( "Video", "MagnificationFilter", g_rendererOptions.m_magFilter );
  iniFile.WriteProfileInt( "Video", "Frameskip", g_rendererOptions.m_frameskip );
	iniFile.WriteProfileInt( "Video", "GraphicsFilter", g_rendererOptions.m_FilterType);
  iniFile.WriteProfileInt( "Video", "ScreenRotation", g_rendererOptions.m_screenRotation );
  iniFile.WriteProfileFloat( "Video", "Brightness", options.brightness );		    // brightness of the display
  iniFile.WriteProfileFloat( "Video", "PauseBrightness", options.pause_bright );     // brightness when in pause
	iniFile.WriteProfileFloat( "Video", "Gamma", options.gamma );			        // gamma correction of the display
//	iniFile.WriteProfileInt( "Video", "ColorDepth", options.color_depth );
	iniFile.WriteProfileInt( "VectorOptions", "VectorWidth", options.vector_width );	      // requested width for vector games; 0 means default (640)
	iniFile.WriteProfileInt( "VectorOptions", "VectorHeight", options.vector_height );	    // requested height for vector games; 0 means default (480)
	iniFile.WriteProfileInt( "VectorOptions", "BeamWidth", options.beam );			            // vector beam width
	iniFile.WriteProfileFloat( "VectorOptions", "FlickerEffect", options.vector_flicker );	  // vector beam flicker effect control
	iniFile.WriteProfileFloat( "VectorOptions", "BeamIntensity", options.vector_intensity );  // vector beam intensity
	iniFile.WriteProfileInt( "VectorOptions", "Translucency", options.translucency );      // 1 to enable translucency on vectors
	//iniFile.WriteProfileInt( "VectorOptions", "Antialiasing", options.antialias );		    // 1 to enable antialiasing on vectors

  FLOAT xPercentage, yPercentage;
  GetScreenUsage( &xPercentage, &yPercentage );
  iniFile.WriteProfileFloat( "Video", "ScreenUsage_X", xPercentage );
  iniFile.WriteProfileFloat( "Video", "ScreenUsage_Y", yPercentage );

  FLOAT xPosition, yPosition;
  GetScreenPosition( &xPosition, &yPosition );
  iniFile.WriteProfileFloat( "Video", "ScreenPos_X", xPosition );
  iniFile.WriteProfileFloat( "Video", "ScreenPos_Y", yPosition );

  iniFile.WriteProfileInt( "Video", "Artwork", options.use_artwork );

 
    //-- Write the directory settings -------------------------------------------
  iniFile.WriteProfileString( "Directories", "RomsPath0",          g_FileIOConfig.m_romPath0 );
  iniFile.WriteProfileString( "Directories", "RomsPath1",          g_FileIOConfig.m_romPath1 );
  iniFile.WriteProfileString( "Directories", "RomsPath2",          g_FileIOConfig.m_romPath2 );
  iniFile.WriteProfileString( "Directories", "RomsPath3",          g_FileIOConfig.m_romPath3 );
  iniFile.WriteProfileString("Directories", "ArtPath",             g_FileIOConfig.m_artPath );
  iniFile.WriteProfileString("Directories", "AudioPath",           g_FileIOConfig.m_audioPath );
  iniFile.WriteProfileString("Directories", "ConfigPath",          g_FileIOConfig.m_configPath );
  iniFile.WriteProfileString("Directories", "GeneralPath",         g_FileIOConfig.m_generalPath );
  iniFile.WriteProfileString("Directories", "HDImagePath",         g_FileIOConfig.m_HDImagePath );
  iniFile.WriteProfileString("Directories", "HiScoresPath",        g_FileIOConfig.m_HiScorePath );
  iniFile.WriteProfileString("Directories", "NVRamPath",           g_FileIOConfig.m_NVramPath );
  iniFile.WriteProfileString("Directories", "BackupPath",          g_FileIOConfig.m_romBackupPath );
  iniFile.WriteProfileString("Directories", "ScreenshotPath",      g_FileIOConfig.m_screenshotPath );
  iniFile.WriteProfileString("Directories", "AutoBootSavePath",    g_FileIOConfig.m_autoBootSavePath );
  iniFile.WriteProfileString("Directories", "CurrentShader",	   g_FileIOConfig.m_shaderFileName);
	  	   
  // There's no reason to allow this to be changed, it's totally internal
  //iniFile.WriteProfileString("Directories", "DefaultRomsListPath", g_FileIOConfig.m_DefaultRomListPath );


    //-- ROM List Options --------------------------------------------------------------------
  iniFile.WriteProfileInt( "ROMListOptions", "DisplayMode", g_romListOptions.m_displayMode );
  iniFile.WriteProfileInt( "ROMListOptions", "SortMode", (UINT32)g_romListOptions.m_sortMode );
  iniFile.WriteProfileInt( "ROMListOptions", "ShowROMStatus", g_romListOptions.m_showROMStatus );
  iniFile.WriteProfileInt( "ROMListOptions", "HideFilteredROMs", g_romListOptions.m_hideFiltered );
  iniFile.WriteProfileInt( "ROMListOptions", "FilterMode", g_romListOptions.m_filterMode );
//  iniFile.WriteProfileInt( "ROMListOptions", "Filter_NumPlayers", g_romListOptions.m_numPlayersFilter );

  iniFile.WriteProfileFloat( "ROMListOptions", "CursorPosition", g_persistentLaunchData.m_cursorPosition );
  iniFile.WriteProfileFloat( "ROMListOptions", "PageOffset", g_persistentLaunchData.m_pageOffset );
  iniFile.WriteProfileInt(   "ROMListOptions", "SuperscrollIndex", g_persistentLaunchData.m_superscrollIndex );

		//-- Skin Options ------------------------------------------------------------------------
	iniFile.WriteProfileString( "SkinOptions", "SelectedSkin", g_skinOptions.m_currentSkin );

 




}

//-------------------------------------------------------------
// RemapDriveLetters
//-------------------------------------------------------------
void RemapDriveLetters( void )
{
  
}
 

//-------------------------------------------------------------
//  RequireController
//-------------------------------------------------------------
void RequireController( DWORD number )
{
 
}

//-------------------------------------------------------------
//	GetGamepadState
//-------------------------------------------------------------
const XINPUT_GAMEPAD *GetGamepadState( UINT32 joynum )
{
	return g_inputManager.GetGamepadDeviceState( joynum );
}

//-------------------------------------------------------------
//	GetGamepadCaps
//-------------------------------------------------------------
const XINPUT_CAPABILITIES *GetGamepadCaps( UINT32 joynum )
{
  return g_inputManager.GetGamepadDeviceCaps( joynum );
}


//-------------------------------------------------------------
//	PollGamepads
//-------------------------------------------------------------
void PollGamepads( void )
{
	g_inputManager.PollDevices();
}

//-------------------------------------------------------------
// WaitForAnyButton
//-------------------------------------------------------------
void WaitForAnyButton( void )
{
	g_inputManager.WaitForAnyButton();
}

//-------------------------------------------------------------
// WaitForNoButton
//-------------------------------------------------------------
void WaitForNoButton( void )
{
	g_inputManager.WaitForNoButton();
}

//-------------------------------------------------------------
// WaitForAnyInput
//-------------------------------------------------------------
void WaitForAnyInput( void )
{
	g_inputManager.WaitForAnyInput();
}

//-------------------------------------------------------------
// WaitForNoInput
//-------------------------------------------------------------
void WaitForNoInput( void )
{
	g_inputManager.WaitForNoInput();
}


//-------------------------------------------------------------
//	GetLightgunCalibratedPosition
//-------------------------------------------------------------
void GetLightgunCalibratedPosition( UINT32 player, INT32 *deltax, INT32 *deltay )
{
	 
}


//-------------------------------------------------------------
//	GetLightgunCalibratedPosition
//-------------------------------------------------------------
void GetLightgunCalibratedPositionOld( UINT32 player, INT32 *deltax, INT32 *deltay )
{
  
}

//-------------------------------------------------------------
//	BeginFontRender
//-------------------------------------------------------------
void BeginFontRender( BOOL ClearScreen, fonttype fontType )
{
	LPDIRECT3DDEVICE9 pD3DDevice = g_graphicsManager.GetD3DDevice();
  g_fontType = fontType;

  if( ClearScreen )
  {
		  // Clear the backbuffer
    pD3DDevice->Clear(	0L,																// Count
											  NULL,															// Rects to clear
											  D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,	// Flags
                        D3DCOLOR_XRGB(0,0,0),							// Color
											  1.0f,															// Z
											  0L );															// Stencil
  }

  g_fontSet.GetFont(g_fontType).Begin();
}

//-------------------------------------------------------------
//	FontRender
//-------------------------------------------------------------
void FontRender( INT32 x, INT32 y, UINT32 color, const WCHAR *str, UINT32 flags )
{
	g_fontSet.GetFont(g_fontType).DrawText( (FLOAT)x, (FLOAT)y, color, str, flags );
}

//-------------------------------------------------------------
//	EndFontRender
//-------------------------------------------------------------
void EndFontRender( BOOL present )
{
	LPDIRECT3DDEVICE9 pD3DDevice = g_graphicsManager.GetD3DDevice();
  g_fontSet.GetFont(g_fontType).End();

  if( present )
    pD3DDevice->Present( NULL, NULL, NULL, NULL );
}

//-------------------------------------------------------------
//  RenderProgressBar
//-------------------------------------------------------------
void RenderProgressBar( INT32 left, 
                        INT32 top, 
                        INT32 right, 
                        INT32 bottom, 
                        UINT32 curValue, 
                        UINT32 maxValue, 
                        D3DCOLOR barColor, 
                        D3DCOLOR borderColor,
												D3DCOLOR backgroundColor )
{
 
}

//-------------------------------------------------------------
//  PresentFrame
//-------------------------------------------------------------
void PresentFrame( void )
{
  g_graphicsManager.GetD3DDevice()->Present( NULL, NULL, NULL, NULL );
}


#ifdef _DEBUG
//-------------------------------------------------------------
//	CheckRAM
//-------------------------------------------------------------
void CheckRAM( void )
{
  WCHAR memStr[256];
  MEMORYSTATUS memStatus;
  GlobalMemoryStatus(  &memStatus );

  swprintf( memStr, 
            L"Memory: %lu/%lu",
            memStatus.dwAvailPhys, 
            memStatus.dwTotalPhys );


	g_inputManager.WaitForNoButton();

  while( !(g_inputManager.IsAnyButtonPressed() || g_inputManager.IsAnyKeyPressed()) )
  {
    g_inputManager.PollDevices();
    BeginFontRender( TRUE, FONTTYPE_DEFAULT );
      FontRender( 640, 200, D3DCOLOR_XRGB(255,200,200), L"This is a DEBUG version of MAMEoX!", 2 );
      FontRender( 640, 280, D3DCOLOR_XRGB(255,255,255), L"Mem: Avail/Total", 2 );
      FontRender( 640, 300, D3DCOLOR_XRGB(255,255,255), memStr, 2 );
    EndFontRender( TRUE );
  }
	g_inputManager.WaitForNoButton();
}
#endif


}	// End Extern "C"
 
//-------------------------------------------------------------
//  RenderToTextureStart
//-------------------------------------------------------------
BOOL RenderToTextureStart( RenderToTextureToken_t &token, LPDIRECT3DDEVICE9 pD3DDevice, LPDIRECT3DTEXTURE9 texture, D3DVIEWPORT9 &textureViewpoint )
{
  if( !texture )
    return FALSE;

  token.m_pD3DDevice = pD3DDevice;
  token.m_viewPoint = textureViewpoint;
  token.m_texture = texture;



  return TRUE;
}

//-------------------------------------------------------------
//  RenderToTextureStop
//-------------------------------------------------------------
void RenderToTextureStop( RenderToTextureToken_t &token )
{


}


//-------------------------------------------------------------
//  Enable128MegCaching
//-------------------------------------------------------------
void Enable128MegCaching( void )
{
 
}
 
