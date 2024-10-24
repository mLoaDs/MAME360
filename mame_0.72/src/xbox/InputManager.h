/**
	* \file		InputManager.h
	*
	*/
#pragma once


//= I N C L U D E S ===========================================================
#include "xbox_Mame.h" 
#include "Gamepad.h"

//= D E F I N E S =============================================================
  // Lighgun calibration step giving the UL corner numbers
#define CALIB_UL    1

  // Lightgun calibration step giving the center numbers
#define CALIB_C     0


//= C L A S S E S =============================================================
class CInputManager
{
public:
		//------------------------------------------------------
		//	Constructor
		//------------------------------------------------------
  CInputManager( void ) :
    m_maxGamepadDevices( 0 ),
	  m_maxMemUnitDevices( 0 ),
    m_maxKeyboardDevices( 0 ),
	  m_gamepadDeviceBitmap( 0 ),
	  m_memunitDeviceBitmap( 0 ),
    m_keyboardDeviceBitmap( 0 ) {
	}


		//------------------------------------------------------
		//	Create
    //! Set up the InputManager instance
    //!
    //! \param    maxGamepads - The maximum number of gamepads
    //!                         supported by the app.
    //!
    //! \param    maxMemUnits - The maximum number of memory
    //!                         units supported by the app.
		//------------------------------------------------------
	BOOL Create( DWORD maxGamepads = 4, DWORD maxMemUnits = 0, BOOL useKeyboard = FALSE ) {
		if( m_created )
			return FALSE;

		if( !maxGamepads )
			return FALSE;

		m_maxGamepadDevices = maxGamepads;
		m_maxMemUnitDevices = maxMemUnits;
        m_maxKeyboardDevices = 0;
 
		// Get the list of devices currently attached to the system
		m_gamepadDeviceBitmap = 1;
		m_memunitDeviceBitmap = 0;
        m_keyboardDeviceBitmap = 0; 
    
		maxMemUnits = 0;

	 // Create the gamepads
		for( UINT32 i = 0; i < 4; ++i )
		{
		  m_gamepads[i].Create( i, 
								maxMemUnits > 2 ? 2 : maxMemUnits, 
								this );

		  if( maxMemUnits > 2 )
			maxMemUnits -= 2;
		  else
			maxMemUnits = 0;
		}


		return TRUE;
	}

inline void AttachRemoveDevices( void ) {
			// Attach/Remove gamepads
   
		
 
	}

 
	void PollDevices( void ) {
    PollGamepadDevices();    
	}

		//------------------------------------------------------
		//	PollGamepadDevices
		//------------------------------------------------------
  void PollGamepadDevices( void ) {
	DWORD insertions = 0;
	DWORD removals = 0;

	m_gamepads[0].PollDevice();
	m_gamepads[1].PollDevice();
	m_gamepads[2].PollDevice();
	m_gamepads[3].PollDevice();

  }

		//------------------------------------------------------
		//	PollKeyboardDevice
		//------------------------------------------------------
  void PollKeyboardDevice( void ) {
	 
  }



		//------------------------------------------------------
		//	WaitForControllerInsertion
    //! Wait until a given controller is inserted
    //!
    //! \param    device - The device number to query (0-3)
		//------------------------------------------------------
  void WaitForControllerInsertion( DWORD device ) {
    if( device < 4 )
    {
      while( !m_gamepads[device].IsConnected() )
        PollGamepadDevices();
    }
  }

		//------------------------------------------------------
		//	IsGamepadConnected
    //! Check to see if a given controller is inserted
    //!
    //! \param    device - The device number to query (0-3)
    //!
    //! \retval   BOOL - TRUE if gamepad is inserted, FALSE otherwise
		//------------------------------------------------------
  BOOL IsGamepadConnected( DWORD device ) const {
    if( device > 3 )
      return FALSE;

    return m_gamepads[device].IsConnected();
  }

		//------------------------------------------------------
		//	GetGamepad
    //! Return a given gamepad instance
    //!
    //! \param    device - The device number to query (0-3)
    //!
    //! \retval   CGamepad * - The requested gamepad object
		//------------------------------------------------------
  CGamepad *GetGamepad( DWORD device ) {
		if( device > 3 )
			return NULL;

    return &m_gamepads[device];
  }

		//------------------------------------------------------
		//	GetGamepad (const version)
    //! Return a given gamepad instance
    //!
    //! \param    device - The device number to query (0-3)
    //!
    //! \retval   const CGamepad * - The requested gamepad object
		//------------------------------------------------------
  const CGamepad *GetGamepad( DWORD device ) const {
		if( device > 3 )
			return NULL;

    if( m_gamepads[device].IsConnected() )
      return &m_gamepads[device];

    return NULL;
  }

		//------------------------------------------------------
		//	GetGamepadDeviceState
    //! Return the current state of a given gamepad
    //!
    //! \param    device - The device number to query (0-3)
    //!
    //! \retval   const XINPUT_GAMEPAD * - The requested
    //!                                    gamepad state object
		//------------------------------------------------------
	const XINPUT_GAMEPAD *GetGamepadDeviceState( DWORD device ) const {
		if( device > 3 )
			return NULL;

    return m_gamepads[device].GetGamepadDeviceState();
	}

		//------------------------------------------------------
		//	GetGamepadDeviceCaps
    //! Return the capabilities of a given gamepad
    //!
    //! \param    device - The device number to query (0-3)
    //!
    //! \retval   const XINPUT_CAPABILITIES * - The requested
    //!                                    gamepad caps object
		//------------------------------------------------------
	const XINPUT_CAPABILITIES *GetGamepadDeviceCaps( DWORD device ) const {
		if( device > 3 )
			return NULL;

    return m_gamepads[device].GetGamepadDeviceCaps();
	}

		//------------------------------------------------------
		//	SetFeedbackState
    //! Send a force feedback effect to a gamepad
    //!
    //! \param  deviceNumber - The gamepad to send to (0-3)
    //! \param  feedback - Struct describing the effect to send
		//------------------------------------------------------
	inline BOOL SetDeviceFeedbackState( DWORD deviceNumber, const XINPUT_FF_EFFECT &feedback ) {
    if( deviceNumber > 3 )
      return FALSE;

    return FALSE;
	}

		//------------------------------------------------------
		//	WaitForAnyButton
		//! \brief		Wait for any button to be pressed on the
		//!            selected joypad
		//!
		//! \param		gamepadNum - Joypad to test (0xFF = all)
		//------------------------------------------------------
	void WaitForAnyButton( DWORD gamepadNum = 0xFF, BOOL checkKeyboard = TRUE ) {
		while( 1 )
		{
			PollDevices();

		if( ((gamepadNum == 0 || gamepadNum == 0xFF) && m_gamepads[0].IsAnyButtonPressed()) ||
          ((gamepadNum == 1 || gamepadNum == 0xFF) && m_gamepads[1].IsAnyButtonPressed()) ||
          ((gamepadNum == 2 || gamepadNum == 0xFF) && m_gamepads[2].IsAnyButtonPressed()) ||
          ((gamepadNum == 3 || gamepadNum == 0xFF) && m_gamepads[3].IsAnyButtonPressed()))
        return;
		}
	}

		//------------------------------------------------------
		//	WaitForNoButton
		//! \brief		Wait for all buttons to be released on the
		//!            selected joypad
		//!
		//! \param		gamepadNum - Joypad to test (0xFF = all)
		//------------------------------------------------------
	void WaitForNoButton( DWORD gamepadNum = 0xFF, BOOL checkKeyboard = TRUE ) {
		BOOL keyPressed = FALSE;
		do
		{
			keyPressed = FALSE;
			PollDevices();

			if( gamepadNum == 0 || gamepadNum == 0xFF )
				keyPressed |= m_gamepads[0].IsAnyButtonPressed();

			if( gamepadNum == 1 || gamepadNum == 0xFF )
				keyPressed |= m_gamepads[1].IsAnyButtonPressed();

			if( gamepadNum == 2 || gamepadNum == 0xFF )
				keyPressed |= m_gamepads[2].IsAnyButtonPressed();

			if( gamepadNum == 3 || gamepadNum == 0xFF )
				keyPressed |= m_gamepads[3].IsAnyButtonPressed();
 

		} while( keyPressed );
	}

		//------------------------------------------------------
		//	WaitForAnyInput
		//! \brief		Wait for anything to be pressed/moved 
		//!            on the selected joypad
		//!
		//! \param		gamepadNum - Joypad to test (0xFF = all)
		//------------------------------------------------------
	void WaitForAnyInput( DWORD gamepadNum = 0xFF, BOOL checkKeyboard = TRUE ) {
		BOOL keyPressed = FALSE;
		do
		{
			keyPressed = FALSE;
			PollDevices();

			if( gamepadNum == 0 || gamepadNum == 0xFF )
				keyPressed |= m_gamepads[0].GetInputState();

			if( gamepadNum == 1 || gamepadNum == 0xFF )
				keyPressed |= m_gamepads[1].GetInputState();

			if( gamepadNum == 2 || gamepadNum == 0xFF )
				keyPressed |= m_gamepads[2].GetInputState();

			if( gamepadNum == 3 || gamepadNum == 0xFF )
				keyPressed |= m_gamepads[3].GetInputState(); 

		} while( !keyPressed );
	}

		//------------------------------------------------------
		//	WaitForNoInput
		//! \brief		Wait for everything to be released on the
		//!            selected joypad
		//!
		//! \param		gamepadNum - Joypad to test (0xFF = all)
		//------------------------------------------------------
	void WaitForNoInput( DWORD gamepadNum = 0xFF, BOOL checkkeyboard = TRUE ) {
		BOOL keyPressed = FALSE;
		do
		{
			keyPressed = FALSE;
			PollDevices();

			if( gamepadNum == 0 || gamepadNum == 0xFF )
				keyPressed |= m_gamepads[0].GetInputState();

			if( gamepadNum == 1 || gamepadNum == 0xFF )
				keyPressed |= m_gamepads[1].GetInputState();

			if( gamepadNum == 2 || gamepadNum == 0xFF )
				keyPressed |= m_gamepads[2].GetInputState();

			if( gamepadNum == 3 || gamepadNum == 0xFF )
				keyPressed |= m_gamepads[3].GetInputState();

		} while( keyPressed );
	}
 
	BOOL IsAnyInput( DWORD gamepadNum = 0xFF, BOOL checkKeyboard = TRUE ) {
    UINT32 ret = FALSE;
    if( gamepadNum == 0 || gamepadNum == 0xFF )
      ret |= m_gamepads[0].GetInputState();
    if( gamepadNum == 1 || gamepadNum == 0xFF )
      ret |= m_gamepads[1].GetInputState();
    if( gamepadNum == 2 || gamepadNum == 0xFF )
      ret |= m_gamepads[2].GetInputState();
    if( gamepadNum == 3 || gamepadNum == 0xFF )
      ret |= m_gamepads[3].GetInputState();
 

    return (ret != 0);
	}
 
  BOOL IsAnyButtonPressed( void ) {
    return  m_gamepads[0].IsAnyButtonPressed() ||
            m_gamepads[1].IsAnyButtonPressed() ||
            m_gamepads[2].IsAnyButtonPressed() ||
            m_gamepads[3].IsAnyButtonPressed();
  }
 
  BOOL IsAnyKeyPressed( void ) {
 
    return FALSE;
  }
 
  BOOL IsButtonPressed( UINT32 buttonID ) {
    return  m_gamepads[0].IsButtonPressed(buttonID) ||
            m_gamepads[1].IsButtonPressed(buttonID) ||
            m_gamepads[2].IsButtonPressed(buttonID) ||
            m_gamepads[3].IsButtonPressed(buttonID);
  }
 
  BOOL IsOnlyButtonPressed( UINT32 buttonID ) {
    return  m_gamepads[0].IsOnlyButtonPressed(buttonID) ||
            m_gamepads[1].IsOnlyButtonPressed(buttonID) ||
            m_gamepads[2].IsOnlyButtonPressed(buttonID) ||
            m_gamepads[3].IsOnlyButtonPressed(buttonID);
  }
 
  BOOL IsOneOfButtonsPressed( UINT32 buttonID ) {
    return  m_gamepads[0].IsOneOfButtonsPressed(buttonID) ||
            m_gamepads[1].IsOneOfButtonsPressed(buttonID) ||
            m_gamepads[2].IsOneOfButtonsPressed(buttonID) ||
            m_gamepads[3].IsOneOfButtonsPressed(buttonID);
  }
 
  BOOL IsKeyPressed( BYTE virtualKeyCode ) const {
	return FALSE;
  }
 
  BOOL IsOnlyKeyPressed( BYTE virtualKeyCode ) const {
	return FALSE;
  }
 
  BOOL AreAllOfKeysPressed( const BYTE *virtualKeyCodeArray, UINT32 numCodes ) const {
	return FALSE;
  }
 
  BOOL IsOneOfKeysPressed( const BYTE *virtualKeyCodeArray, UINT32 numCodes ) const {
	return FALSE;
  }



  DWORD GetGamepadDeviceBitmap( void ) const { return m_gamepadDeviceBitmap; }
  DWORD GetMUDeviceBitmap( void ) const { return m_memunitDeviceBitmap; }
  DWORD GetKeyboardDeviceBitmap( void ) const { return m_keyboardDeviceBitmap; }

protected:

  static BOOL			m_created;									//!<	Whether or not this singleton has been created

  DWORD			  m_maxGamepadDevices;				//!<	The max number of gamepad devices to poll
  DWORD			  m_maxMemUnitDevices;				//!<	The max number of mem units to poll
  DWORD           m_maxKeyboardDevices;       //!<  The max number of keyboards to poll (can only be 0 or 1)

  DWORD			  m_gamepadDeviceBitmap;			//!<	Bitmap storing which gamepad devices are currently attached
  DWORD			  m_memunitDeviceBitmap;			//!<	Bitmap storing which mem unit devices are currently attached
  DWORD           m_keyboardDeviceBitmap;     //!<	Bitmap storing which keyboard devices are currently attached

  CGamepad        m_gamepads[4];              //!<  The gamepad objects
};
