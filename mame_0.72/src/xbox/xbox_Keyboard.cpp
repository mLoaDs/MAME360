/**
	* \file			xbox_Keyboard.c
	* \brief		Implementation of the "Keyboard" section of
	*           osdepend.h
	*/

//= I N C L U D E S ====================================================
#include "osd_cpu.h"
#include "osdepend.h"
#include <assert.h>

#include "xbox_Keyboard.h"
#include "InputManager.h"
#include "DebugLogger.h"

//= D E F I N E S ======================================================
#define MAX_SUPPORTED_KEYS    128

  // Helper macro, calls Helper_AddEntry
#define BEGINENTRYMAP()                                  UINT32 keycount = 0;
#define ADDENTRY( _name__, _code__, _standardCode__ )    Helper_AddEntry( (_name__), (_code__), (_standardCode__), &keycount )

//= G L O B A L = V A R S ==============================================
extern CInputManager			  g_inputManager; // Defined in MAMEoXUtil.cpp


  // number of OSD input keywords we've added (defined in xbox_Input.c)
extern "C" {
  extern UINT32					  g_numOSDInputKeywords;
}

  // The keyboard info struct for the MAME core
static struct KeyboardInfo		g_keyboardInfo[MAX_SUPPORTED_KEYS] = { 0 };

//= P R O T O T Y P E S ================================================
	//------------------------------------------------------------------
	//	Helper_AddEntry
  //! \brief    Adds an entry into the g_keyboardInfo array
  //!
  //! \param    name - The user friendly name of the entry
  //! \param    code - The OSD-specific identifier of the entry
  //! \param    standardCode - The "standard" input code of the entry (see input.h in the MAME tree)
  //! \param    keycount - [IN,OUT] The index in the KeyboardInfo array to modify. Incremented on success
  //!
  //! \note     standardCode may be set to a value obtained via joyoscode_to_code for any codes 
  //!           w/out a specific entry in the keycode/keyboard enum (defined in input.h of the MAME tree)
	//------------------------------------------------------------------
static void Helper_AddEntry( const char *name, BYTE code, INT32 standardCode, UINT32 *keycount );

//= F U N C T I O N S ==================================================

//---------------------------------------------------------------------
//	osd_get_key_list
//---------------------------------------------------------------------
const struct KeyboardInfo *osd_get_key_list( void )
{
/*
  return a list of all available keys (see input.h)
*/
	return g_keyboardInfo;
}

//---------------------------------------------------------------------
//	osd_is_key_pressed
//---------------------------------------------------------------------
int osd_is_key_pressed( int keycode )
{
/*
  tell whether the specified key is pressed or not. keycode is the OS dependant
  code specified in the list returned by osd_get_key_list().
*/
	return g_inputManager.IsKeyPressed( keycode );
}

//---------------------------------------------------------------------
//	osd_readkey_unicode
//---------------------------------------------------------------------
int osd_readkey_unicode( int flush )
{
/*
  Return the Unicode value of the most recently pressed key. This
  function is used only by text-entry routines in the user interface and should
  not be used by drivers. The value returned is in the range of the first 256
  bytes of Unicode, e.g. ISO-8859-1. A return value of 0 indicates no key down.

  Set flush to 1 to clear the buffer before entering text. This will avoid
  having prior UI and game keys leak into the text entry.
*/
//	PRINTMSG(( T_TRACE, "osd_readkey_unicode" ));
	return 0;
}

 

//---------------------------------------------------------------------
//	InitializeKeyboard
//---------------------------------------------------------------------
void InitializeKeyboard( void )
{
 
  
}


//-------------------------------------------------------
//	Helper_AddEntry
//-------------------------------------------------------
static void Helper_AddEntry( const char *name, BYTE code, INT32 standardCode, UINT32 *keycount )
{
   
}

 


