/**
	* \file			xbox_Main.cpp
	* \brief		MAMEoX entry point
	*
	*/


//= I N C L U D E S ====================================================
#include "xbox_Mame.h"
#include <stdarg.h>
#include <stdio.h>
#include <crtdbg.h>

#ifdef _PROFILER
  #include <xbdm.h>
#endif

 
#include "InputManager.h"
#include "GraphicsManager.h"
#include "DebugLogger.h"
#include "FontSet.h"
#include "XBESectionUtil.h"
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


//= S T R U C T U R E S ===============================================
struct CUSTOMVERTEX
{
  D3DXVECTOR3   pos;      // The transformed position for the vertex
  DWORD         diffuse;  // The diffuse color of the vertex   
};

//= G L O B A L = V A R S =============================================
  // Defined in MAMEoXUtil.cpp
extern CInputManager	 g_inputManager;
extern CGraphicsManager	  g_graphicsManager;
extern CFontSet           g_fontSet;

  // XBE Launch data
DWORD													g_launchDataType;
BYTE													g_launchData[MAX_LAUNCH_DATA_SIZE];                 

 

  // Sound processing override
BOOL g_soundEnabled = TRUE;


  // Dummy screensaverTimeout value (only used in the Launcher)
UINT32 g_screensaverTimeout;

//= P R O T O T Y P E S ===============================================
static void Die( LPDIRECT3DDEVICE9 pD3DDevice, const char *fmt, ... );
static BOOL Helper_RunRom( UINT32 romIndex );
static BOOL __cdecl compareDriverNames( const void *elem1, const void *elem2 );
static BOOL Helper_SaveDriverInfoFile( void );
void DrawDriverProgressData( const char *fileName, DWORD index, DWORD total );
static BOOL Helper_IsBIOS( const GameDriver *drv );




//= D E F I N E S  =====================================================
//#define ENABLE_LOGERROR     // Enable the logerror function (this can spit out a _lot_ of data)

extern "C" {
void auto_free(void);
int fatalerror( const char *fmt, ... );
}

//= F U N C T I O N S =================================================


int ExceptionFilter(LPEXCEPTION_POINTERS e)
{
 	 
	fatalerror("The driver could not be loaded. An Exception has occurred. MAME will restart...." );
    return EXCEPTION_CONTINUE_EXECUTION;
}



//-------------------------------------------------------------
//	main
//-------------------------------------------------------------
void main_mame( int gameIndex )
{
 
  DebugLoggerInit();

  MAMEoXLaunchData_t *mameoxLaunchData = (MAMEoXLaunchData_t*)g_launchData;

  //LoadOptions();      // Must be done before graphics mgr (for VSYNC) and before input mgr (lightgun calib data)
  InitializeFileIO(); // Initialize fileIO (must be done after LoadOptions!)


  //-----------------------------

  InitDriverSectionizer();
  InitCPUSectionizer();

  if( mameoxLaunchData->m_command == LAUNCH_CREATE_MAME_GAME_LIST )
  {
		mameoxLaunchData->m_gameIndex = 0;

  }
  else
  {

    qsort( drivers, mameoxLaunchData->m_totalMAMEGames, sizeof(drivers[0]), compareDriverNames );

	const char *DriverName = drivers[mameoxLaunchData->m_gameIndex]->source_file;
	  
	PRINTMSG(( T_INFO, "*** Driver name: %s\n", drivers[mameoxLaunchData->m_gameIndex]->source_file ));

		// VC6 seems to be calling this with the full path so strstr just trims down the path
		// appropriately. NOTE: we probably don't need this to conditionally compile and could
		// just leave the first call but would like someone with VS.net to test first just in case :)
		//
		// [EBA] Should be perfectly fine w/ the strstr
		const char *driverName = strstr(DriverName,"drivers\\");
		if( !driverName || !driverName[8] )
		{
			PRINTMSG(( T_ERROR, "Invalid driver name at index %lu!", mameoxLaunchData->m_gameIndex ));
			return ;
			//fatalerror( "An unrecoverable error has occurred!\r\nInvalid driver name at index %lu!", romIndex );
		}

	if( !LoadDriverSectionByName( &driverName[8] ) )
		{
		PRINTMSG(( T_ERROR, "Failed to load section for file %s!", DriverName ));
		return ;
			//fatalerror( "An unrecoverable error has occurred!\r\nFailed to load XBE section for file %s!", &driverName[8] );
		}

		// Unload all the other XBE data sections, as we'll only be using the one
		// for the file we're loading
		// Note that we do this _after_ the LoadDriverSectionByName.
		// This increments the refcount on the driver causing the system 
		// not to release anything allocated by the segment back to the heap
		// (MAME shouldn't allocate anything at this point, but it's good to be safe)
	UnloadDriverSections();
	//UnloadCPUSections();          // CPU sections are unloaded in mame/src/cpuexec.c

	// Free up unneeded sectionizer memory
	TerminateDriverSectionizer();

  }
 
	  // Initialize the input subsystem
	g_inputManager.Create( 4, 0, TRUE );  // 4 controllers, no memory cards, allow keyboard

	  // Intialize the various MAMEoX subsystems
  InitializeTiming();
  InitializeD3DRenderer( g_graphicsManager, &g_fontSet.DefaultFont() );
  
  CHECKRAM();

    // Register the loadable section names for lookup at runtime
  //InitDriverSectionizer();
  //InitCPUSectionizer();
 
  DEBUGGERCHECKRAM()

    // Create the sorted game listing and exit
  if( mameoxLaunchData->m_command == LAUNCH_CREATE_MAME_GAME_LIST )
  {
	    // Count and sort the game drivers (Taken from XMame)
    DWORD totalMAMEGames = 0;
	  for( totalMAMEGames = 0; drivers[totalMAMEGames]; ++totalMAMEGames)
		  ;
    mameoxLaunchData->m_totalMAMEGames = totalMAMEGames;
    mameoxLaunchData->m_gameIndex = 0;

    qsort( drivers, mameoxLaunchData->m_totalMAMEGames, sizeof(drivers[0]), compareDriverNames );

      // Dump the drivers to a file
    Helper_SaveDriverInfoFile();
  }
  else
  {
 
	 mameoxLaunchData->m_gameIndex = gameIndex;

	__try {		
		
		Helper_RunRom( mameoxLaunchData->m_gameIndex );


    } 
	__except(ExceptionFilter(GetExceptionInformation()))
	{

		

	}
	 
	SaveOptions();
	D3DRendererDestroySession();

	 
  }

}


//-------------------------------------------------------------
//  Helper_IsBIOS
//-------------------------------------------------------------
static BOOL Helper_IsBIOS( const GameDriver *drv )
{
  if( !drv )
    return FALSE;

    // The list of bios drivers from www.mame.dk
  if( !stricmp( drv->name, "decocass" ) ||
      !stricmp( drv->name, "cvs" ) ||
      !stricmp( drv->name, "neogeo" ) ||
      !stricmp( drv->name, "pgm" ) ||
      !stricmp( drv->name, "playch10" ) ||
      !stricmp( drv->name, "stvbios" ) ||
      !stricmp( drv->name, "skns" ) ||
      !stricmp( drv->name, "konamigx" ) )
    return TRUE;

  return FALSE;
}


//-------------------------------------------------------------
//  Helper_RunRom
//-------------------------------------------------------------
static BOOL Helper_RunRom( UINT32 romIndex )
{
  BOOL ret;
	options.ui_orientation = drivers[romIndex]->flags & ORIENTATION_MASK;

    // Because the D3D renderer will be rotating the output into place, 
    //  we have to set up the UI w/ options such that it will produce
    //  graphics inline w/ those produced by the ROM itself (otherwise
    //  the renderer will rotate "correct" UI into an incorrect orientation)
	if( options.ui_orientation & ORIENTATION_SWAP_XY )
	{
		  // if only one of the components is inverted, swap which is inverted
		if( (options.ui_orientation & ROT180) == ORIENTATION_FLIP_X ||
				(options.ui_orientation & ROT180) == ORIENTATION_FLIP_Y)
			options.ui_orientation ^= ROT180;
	}

  #ifdef _PROFILER
  DmStartProfiling( "xd:\\perf.log", 0 );
  #endif
 
    // Override sound processing
  DWORD samplerate = options.samplerate;
  if( !g_soundEnabled )
    options.samplerate = 0;
 
    MEMORYSTATUS memStatus;
    GlobalMemoryStatus(  &memStatus );
    PRINTMSG(( T_INFO, 
              "Memory: %lu/%lu",
              memStatus.dwAvailPhys, 
              memStatus.dwTotalPhys ));
 
	ret = run_game( romIndex );

    // Restore the old value, just in case somebody writes to the INI
  options.samplerate = samplerate;

  #ifdef _PROFILER
  DmStopProfiling();
  #endif

  return (!ret);  // run_game works on inverted logic
}

//-------------------------------------------------------------
//	Die
//-------------------------------------------------------------
static void Die( LPDIRECT3DDEVICE9 pD3DDevice, const char *fmt, ... )
{
	char buf[1024];
  WCHAR wBuf[1024];

  va_list arg;
  va_start( arg, fmt );
  vsprintf( buf, fmt, arg );
  va_end( arg );

	PRINTMSG(( T_ERROR, "Die: %s", buf ));
	g_inputManager.WaitForNoButton();

  while( !(g_inputManager.IsAnyButtonPressed() || g_inputManager.IsAnyKeyPressed()) )
  {
    g_inputManager.PollDevices();

		  // Display the error to the user
	  pD3DDevice->Clear(	0L,																// Count
											  NULL,															// Rects to clear
											  D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,	// Flags
											  D3DCOLOR_XRGB(0,0,0),							// Color
											  1.0f,															// Z
											  0L );															// Stencil

	  g_fontSet.DefaultFont().Begin();    	
	    mbstowcs( wBuf, buf, strlen(buf) + 1 );
	    g_fontSet.DefaultFont().DrawText( 640, 150, D3DCOLOR_RGBA( 255, 255, 255, 255), wBuf, ATGFONT_CENTER_X | ATGFONT_CENTER_Y );
	    g_fontSet.DefaultFont().DrawText( 640, 508, D3DCOLOR_RGBA( 255, 125, 125, 255), L"Press any button to reboot.", ATGFONT_CENTER_X | ATGFONT_CENTER_Y );

	  g_fontSet.DefaultFont().End();
	  pD3DDevice->Present( NULL, NULL, NULL, NULL );
  }

  g_inputManager.WaitForNoButton();

    // Make sure MAMEoXLauncher acts as though it was launched from the dashboard
  MAMEoXLaunchData_t *mameoxLaunchData = (MAMEoXLaunchData_t*)g_launchData;
  mameoxLaunchData->m_command = LAUNCH_RUN_AS_IF_REBOOTED;

    // Relaunch MAMEoXLauncher
  ShowLoadingScreen( pD3DDevice );

  XSetLaunchData( &g_launchData, sizeof(g_launchData) );
  XLaunchNewImage( "GAME:\\mame.xex", 0 );

  while( !(g_inputManager.IsAnyButtonPressed() || g_inputManager.IsAnyKeyPressed()) )
  {
    g_inputManager.PollDevices();

		  // Display the error to the user
	  pD3DDevice->Clear(	0L,																// Count
											  NULL,															// Rects to clear
											  D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,	// Flags
											  D3DCOLOR_XRGB(0,0,0),							// Color
											  1.0f,															// Z
											  0L );															// Stencil
    g_fontSet.DefaultFont().Begin();
      swprintf( wBuf, L"Failed to launch GAME:\\mame.xex 0x%X", 0 );
	    g_fontSet.DefaultFont().DrawText( 640, 150, D3DCOLOR_RGBA( 255, 255, 255, 255), wBuf, ATGFONT_CENTER_X | ATGFONT_CENTER_Y );
	    g_fontSet.DefaultFont().DrawText( 640, 508, D3DCOLOR_RGBA( 255, 125, 125, 255), L"Press any button to reboot.", ATGFONT_CENTER_X | ATGFONT_CENTER_Y );
	  g_fontSet.DefaultFont().End();
	  pD3DDevice->Present( NULL, NULL, NULL, NULL );
  }
  g_inputManager.WaitForNoButton();

    // Attempt to get back to the dashboard
 

	XLaunchNewImage( XLAUNCH_KEYWORD_DEFAULT_APP, 0 );


  g_fontSet.DefaultFont().Begin();
    swprintf( wBuf, L"Failed to launch the dashboard! 0x%X", 0 );
  g_fontSet.DefaultFont().DrawText( 640, 150, D3DCOLOR_RGBA( 255, 255, 255, 255), wBuf, ATGFONT_CENTER_X | ATGFONT_CENTER_Y );
  g_fontSet.DefaultFont().DrawText( 640, 508, D3DCOLOR_RGBA( 255, 255, 255, 255), L"You need to power off manually!", ATGFONT_CENTER_X | ATGFONT_CENTER_Y );
	g_fontSet.DefaultFont().End();
	pD3DDevice->Present( NULL, NULL, NULL, NULL );
}

//-------------------------------------------------------------
//  Helper_SaveDriverInfoFile
//-------------------------------------------------------------
static BOOL Helper_SaveDriverInfoFile( void )
{
  return TRUE;
}

//-------------------------------------------------------------
//  DrawDriverProgressData
//-------------------------------------------------------------
void DrawDriverProgressData( const char *fileName, DWORD index, DWORD total )
{
	LPDIRECT3DDEVICE9 pD3DDevice = g_graphicsManager.GetD3DDevice();

	if ( pD3DDevice == NULL )
		return ;

		// Display the error to the user
	pD3DDevice->Clear(	0L,																// Count
											NULL,															// Rects to clear
											D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,	// Flags
											D3DCOLOR_XRGB(56,0,240),							// Color
											1.0f,															// Z
											0L );															// Stencil

	g_fontSet.DefaultFont().Begin();

	g_fontSet.DefaultFont().DrawText( 640, 320, D3DCOLOR_XRGB( 255, 255, 255 ), L"MAME 0.72 Release 2", ATGFONT_CENTER_X | ATGFONT_CENTER_Y );
  	g_fontSet.DefaultFont().DrawText( 640, 360, D3DCOLOR_XRGB( 255, 255, 255 ), L"Loading Roms Please Wait....", ATGFONT_CENTER_X | ATGFONT_CENTER_Y );

    // Draw the current filename

	WCHAR wBuf[256];
	mbstowcs( wBuf, fileName, 256 );
	g_fontSet.DefaultFont().DrawText( 640, 418, D3DCOLOR_XRGB( 255, 255, 255 ), wBuf, ATGFONT_CENTER_X | ATGFONT_CENTER_Y );

	g_fontSet.DefaultFont().End();


	
	pD3DDevice->Present( NULL, NULL, NULL, NULL );
}

//-------------------------------------------------------------
//	compareDriverNames
//-------------------------------------------------------------
static BOOL __cdecl compareDriverNames( const void *elem1, const void *elem2 )
{
	struct GameDriver *drv1 = *(struct GameDriver **)elem1;
	struct GameDriver *drv2 = *(struct GameDriver **)elem2;

		// Test the description field (game name)
	return strcasecmp( drv1->description, drv2->description );
}

extern "C" {
//-------------------------------------------------------------
//	osd_init
//-------------------------------------------------------------
int osd_init( void )
{

	return 0;
}

//-------------------------------------------------------------
//	osd_exit
//-------------------------------------------------------------
void osd_exit( void )
{
//  TerminateJoystickMouse(); // Unnecessary as we'll just exit anyway
}

//---------------------------------------------------------------------
//	fatalerror
//---------------------------------------------------------------------
int fatalerror( const char *fmt, ... )
{
  wchar_t wBuf[1024];
  char buf[1024];

  va_list arg;
  va_start( arg, fmt );
  vsprintf( buf, fmt, arg );
  va_end( arg );

  mbstowcs( wBuf, buf, 1023 );

	g_inputManager.WaitForNoButton();

	LPDIRECT3DDEVICE9 pD3DDevice = g_graphicsManager.GetD3DDevice();

  while( !(g_inputManager.IsAnyButtonPressed() || g_inputManager.IsAnyKeyPressed()) )
  {
    g_inputManager.PollDevices();

		  // Display the error to the user
	  pD3DDevice->Clear(	0L,																// Count
											  NULL,															// Rects to clear
											  D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,	// Flags
											  D3DCOLOR_XRGB(0,0,0),							// Color
											  1.0f,															// Z
											  0L );															// Stencil

	  g_fontSet.DefaultFont().Begin();
  	
      g_fontSet.DefaultFont().DrawText( 640, 150, D3DCOLOR_RGBA( 255, 255, 255, 255 ), L"Fatal Error:", ATGFONT_CENTER_X | ATGFONT_CENTER_Y );
	  g_fontSet.DefaultFont().DrawText( 640, 275, D3DCOLOR_RGBA( 255, 255, 255, 255 ), wBuf, ATGFONT_CENTER_X | ATGFONT_CENTER_Y );
	  g_fontSet.DefaultFont().DrawText( 640, 508, D3DCOLOR_RGBA( 255, 255, 255, 255), L"Press any button to continue.", ATGFONT_CENTER_X | ATGFONT_CENTER_Y );
	  g_fontSet.DefaultFont().End();

	  pD3DDevice->Present( NULL, NULL, NULL, NULL );
  }

  g_inputManager.WaitForNoButton();

    // Relaunch MAMEoXLauncher
  ShowLoadingScreen( pD3DDevice );
  XSetLaunchData( &g_launchData, sizeof(g_launchData) );
  XLaunchNewImage( "GAME:\\mame.xex", 0 );
  Die( pD3DDevice, "Failed to launch GAME:\\mame.xex! 0x%X", 0 );

    // Execution should never get here
  return 0;
}


//-------------------------------------------------------------
//	ShowLoadingScreen
//-------------------------------------------------------------
void ShowLoadingScreen( LPDIRECT3DDEVICE9 pD3DDevice )
{

		// Clear the backbuffer
  pD3DDevice->Clear(	0L,																// Count
											NULL,															// Rects to clear
											D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,	// Flags
                      D3DCOLOR_XRGB(0,0,0),							// Color
											1.0f,															// Z
											0L );															// Stencil

  g_fontSet.DefaultFont().Begin();
  g_fontSet.DefaultFont().DrawText( 640, 230, D3DCOLOR_RGBA( 255, 255, 255, 255),   L"Loading. Please wait...", ATGFONT_CENTER_X | ATGFONT_CENTER_Y );
  g_fontSet.DefaultFont().End();

  pD3DDevice->Present( NULL, NULL, NULL, NULL );
}

}	// End Extern "C"

