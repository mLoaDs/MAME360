/**
	* \file		Gamepad.cpp
  *         Simple wrapper around the XINPUT_STATE and XINPUT_CAPABILITIES
  *         structures, providing utility functions such as IsButtonPressed
	*/


//= I N C L U D E S ===========================================================
#include "Gamepad.h"
#include "InputManager.h"
#include "DebugLogger.h"


//= D E F I N E S =============================================================
#define STICK_DEADZONE              0.38f

//= G L O B A L = V A R S =====================================================


//= F U N C T I O N S =================================================



//------------------------------------------------------
//	Constructor
//------------------------------------------------------
CGamepad::CGamepad( void ) : CInputDevice()
{
	m_topMemPortMask = m_bottomMemPortMask = 0;
	m_memunitDeviceHandles[0] = m_memunitDeviceHandles[1] = NULL;
}

//------------------------------------------------------
//	Create
//------------------------------------------------------
BOOL CGamepad::Create(  DWORD gpIndex, 
                        DWORD maxMemUnits, 
                        CInputManager *inputManager )
{
  if( gpIndex > 3 || !inputManager )
    return FALSE;

  if( !CInputDevice::Create( gpIndex, inputManager ) )
    return FALSE;

	switch( m_portIndex )
	{
	default:
//			assert( 0 && "Invalid gamepad port number!" );
		// fallthrough

		// *** 0 *** //
	case 0:
		m_portName          = 0;
		m_portMask          = 0;
		m_topMemPortMask    = 0;
		m_bottomMemPortMask = 0;
		m_deviceHandle = (HANDLE)0x00;
		break;

		// *** 1 *** //
	case 1:
		m_portName          = 1;
		m_portMask          = 1;
		m_topMemPortMask    = 1;
		m_bottomMemPortMask = 1;
		m_deviceHandle = (HANDLE)0x01;
		break;

		// *** 2 *** //
	case 2:
		m_portName          = 2;
		m_portMask          = 2;
		m_topMemPortMask    = 2;
		m_bottomMemPortMask = 2;
		m_deviceHandle = (HANDLE)0x02;
		break;

		// *** 3 *** //
	case 3:
		m_portName          = 3;
		m_portMask          = 3;
		m_topMemPortMask    = 3;
		m_bottomMemPortMask = 3;
		m_deviceHandle = (HANDLE)0x03;
		break;
	}

  return TRUE;
}

//------------------------------------------------------
//	PollDevice
//------------------------------------------------------
void CGamepad::PollDevice( void )
{

  m_inputManager->AttachRemoveDevices();
  
  XInputGetState( (DWORD)m_deviceHandle, &m_state );
  
}

//------------------------------------------------------
//	WaitForAnyButton
//------------------------------------------------------
void CGamepad::WaitForAnyButton( void ) 
{
  if( !m_deviceHandle )
  {
    PRINTMSG(( T_ERROR, "WaitForAnyButton called on invalid gamepad!" ));
    return;
  }

  do
  {
    PollDevice();
  } while( !IsAnyButtonPressed() );
}

//------------------------------------------------------
//	WaitForNoButton
//------------------------------------------------------
void CGamepad::WaitForNoButton( void ) 
{
  if( !m_deviceHandle )
  {
    PRINTMSG(( T_ERROR, "WaitForNoButton called on invalid gamepad!" ));
    return;
  }

	do
	{
		PollDevice();
  } while( IsAnyButtonPressed() );
}

//------------------------------------------------------
//	WaitForAnyInput
//------------------------------------------------------
void CGamepad::WaitForAnyInput( void ) 
{
  if( !m_deviceHandle )
  {
    PRINTMSG(( T_ERROR, "WaitForAnyInput called on invalid gamepad!" ));
    return;
  }

	do
	{
		PollDevice();
  } while( !GetInputState() );
}

//------------------------------------------------------
//	WaitForNoInput
//------------------------------------------------------
void CGamepad::WaitForNoInput( void ) 
{
  if( !m_deviceHandle )
  {
    PRINTMSG(( T_ERROR, "WaitForNoInput called on invalid gamepad!" ));
    return;
  }

	do
	{
		PollDevice();
  } while( GetInputState() );
}




//------------------------------------------------------
//	IsMUConnected
//------------------------------------------------------
BOOL CGamepad::IsMUConnected( BOOL bottomMU ) const
{
  return m_memunitDeviceHandles[bottomMU] != NULL;
}

//------------------------------------------------------
//	GetAnalogButtonState
//------------------------------------------------------
UINT8 CGamepad::GetAnalogButtonState( gamepadButtonID_t buttonID ) const
{
  if( !IsConnected() )
    return 0;

  const XINPUT_GAMEPAD	&gp = m_state.Gamepad;
  switch( buttonID )
  {
  case GP_A:
    return gp.wButtons & XINPUT_GAMEPAD_A;

  case GP_B:
    return gp.wButtons & XINPUT_GAMEPAD_B;

  case GP_X:
    return gp.wButtons & XINPUT_GAMEPAD_X;

  case GP_Y:
    return gp.wButtons & XINPUT_GAMEPAD_Y;
 
  case GP_LEFT_TRIGGER:
    return gp.bLeftTrigger;  

  case GP_RIGHT_TRIGGER:
    return gp.bRightTrigger;
  }
  return 0;
}

//------------------------------------------------------
//	GetAnalogAxisState
//------------------------------------------------------
SHORT CGamepad::GetAnalogAxisState( gamepadAnalogID_t analogID, gamepadAxisID_t axisID ) const
{
  if( !IsConnected() )
    return 0;

  const XINPUT_GAMEPAD &gp = m_state.Gamepad;
  if( analogID == GP_ANALOG_LEFT )
  {
    if( axisID == GP_AXIS_X )
      return gp.sThumbLX;
    else
      return gp.sThumbLY;
  }
  else
  {
    if( axisID == GP_AXIS_X )
      return gp.sThumbRX;
    else
      return gp.sThumbRY;
  }
}

//------------------------------------------------------
//	GetInputState
//------------------------------------------------------
UINT32 CGamepad::GetInputState( void ) const
{
 
  const XINPUT_GAMEPAD &gp = m_state.Gamepad;
	UINT32 curState;

	curState = 0;
	curState |= ( ( gp.wButtons & XINPUT_GAMEPAD_DPAD_UP ) != 0 ) ?    GP_DPAD_UP : 0;
	curState |= ( ( gp.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT ) != 0 ) ? GP_DPAD_RIGHT : 0;
	curState |= ( ( gp.wButtons & XINPUT_GAMEPAD_DPAD_DOWN ) != 0 ) ?  GP_DPAD_DOWN : 0;
	curState |= ( ( gp.wButtons & XINPUT_GAMEPAD_DPAD_LEFT ) != 0 ) ?  GP_DPAD_LEFT : 0;

	curState |= ( ( gp.wButtons & XINPUT_GAMEPAD_BACK ) != 0 ) ?  GP_BACK : 0;
	curState |= ( ( gp.wButtons & XINPUT_GAMEPAD_START ) != 0 ) ? GP_START : 0;

	curState |= ( ( gp.wButtons & XINPUT_GAMEPAD_LEFT_THUMB ) != 0 ) ?  GP_LEFT_ANALOG : 0;
	curState |= ( ( gp.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB ) != 0 ) ? GP_RIGHT_ANALOG : 0;

	curState |= ( (gp.wButtons & XINPUT_GAMEPAD_A) >= BUTTON_PRESS_THRESHOLD ) ? GP_A : 0;
	curState |= ( (gp.wButtons & XINPUT_GAMEPAD_B) >= BUTTON_PRESS_THRESHOLD ) ? GP_B : 0;
	curState |= ( (gp.wButtons & XINPUT_GAMEPAD_X) >= BUTTON_PRESS_THRESHOLD ) ? GP_X : 0;
	curState |= ( (gp.wButtons & XINPUT_GAMEPAD_Y) >= BUTTON_PRESS_THRESHOLD ) ? GP_Y : 0;

	curState |= ( ( gp.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER ) != 0 ) ?  GP_BACK : 0;
	curState |= ( ( gp.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER ) != 0 ) ?  GP_BACK : 0;
 
	curState |= ( gp.bLeftTrigger >= BUTTON_PRESS_THRESHOLD ) ? GP_LEFT_TRIGGER : 0;
	curState |= ( gp.bRightTrigger >= BUTTON_PRESS_THRESHOLD ) ? GP_RIGHT_TRIGGER : 0;

  #define ANALOG_AS_DIGITAL_VAL    ( SHORT )( 32767.0f * STICK_DEADZONE )

	curState |= ( gp.sThumbLY >=  ANALOG_AS_DIGITAL_VAL ) ? GP_LA_UP : 0;
	curState |= ( gp.sThumbLX <= -ANALOG_AS_DIGITAL_VAL ) ? GP_LA_LEFT : 0;
	curState |= ( gp.sThumbLY <= -ANALOG_AS_DIGITAL_VAL ) ? GP_LA_DOWN : 0;
	curState |= ( gp.sThumbLX >=  ANALOG_AS_DIGITAL_VAL ) ? GP_LA_RIGHT : 0;

	curState |= ( gp.sThumbRY >=  ANALOG_AS_DIGITAL_VAL ) ? GP_RA_UP : 0;
	curState |= ( gp.sThumbRX <= -ANALOG_AS_DIGITAL_VAL ) ? GP_RA_LEFT : 0;
	curState |= ( gp.sThumbRY <= -ANALOG_AS_DIGITAL_VAL ) ? GP_RA_DOWN : 0;
	curState |= ( gp.sThumbRX >=  ANALOG_AS_DIGITAL_VAL ) ? GP_RA_RIGHT : 0;

  return( curState );
}

//------------------------------------------------------
//	IsAnyButtonPressed
//------------------------------------------------------
BOOL CGamepad::IsAnyButtonPressed( void ) const
{
	UINT32 state = GetInputState();

	  // Only Buttons
	state &= ( GP_A | GP_B | GP_X | GP_Y | 
             GP_BLACK | GP_WHITE | 
             GP_LEFT_TRIGGER | GP_RIGHT_TRIGGER | 
             GP_LEFT_ANALOG | GP_RIGHT_ANALOG |
             GP_START | GP_BACK );

	return( state != 0 );
}

//------------------------------------------------------
//	IsButtonPressed
//------------------------------------------------------
BOOL CGamepad::IsButtonPressed( UINT32 buttonID ) const
{
  return ( (GetInputState() & buttonID) == buttonID );
}

//------------------------------------------------------
//	IsOnlyButtonPressed
//------------------------------------------------------
BOOL CGamepad::IsOnlyButtonPressed( UINT32 buttonID ) const
{
  return ( GetInputState() == buttonID );
}

//------------------------------------------------------
//	IsOneOfButtonsPressed
//------------------------------------------------------
BOOL CGamepad::IsOneOfButtonsPressed( UINT32 buttonID ) const
{
  return ( GetInputState() & buttonID );
}


//------------------------------------------------------
//	GetGamepadDeviceState
//------------------------------------------------------
const XINPUT_GAMEPAD *CGamepad::GetGamepadDeviceState( void ) const 
{
  if( IsConnected() )
	  return &m_state.Gamepad;

  return NULL;
}


//------------------------------------------------------
//	SetLightgunCalibration
//------------------------------------------------------
void CGamepad::SetLightgunCalibration( INT32 cx, INT32 cy, INT32 ulx, INT32 uly )
{
 
}

//------------------------------------------------------
//	GetLightgunFlags
//------------------------------------------------------
DWORD CGamepad::GetLightgunFlags( void ) const
{
 
  return 0;
}

//------------------------------------------------------
//	IsLightgunPointedAtScreen
//------------------------------------------------------
BOOL CGamepad::IsLightgunPointedAtScreen( void ) const
{
  return FALSE;
}

//------------------------------------------------------
//	AttachRemoveDevices
//------------------------------------------------------
void CGamepad::AttachRemoveDevices( void ) 
{
		// Attach/Remove gamepads
	AttachRemoveGamepadDevice();

		// Attach/Remove MemUnit sets
	AttachRemoveMemUnitDevicePair();
}

//------------------------------------------------------
//	AttachRemoveGamepadDevice
//------------------------------------------------------
void CGamepad::AttachRemoveGamepadDevice( void )
{
	
}

//------------------------------------------------------
//	AttachRemoveMemUnitDevicePair
//------------------------------------------------------
void CGamepad::AttachRemoveMemUnitDevicePair( void )
{

	 
}



