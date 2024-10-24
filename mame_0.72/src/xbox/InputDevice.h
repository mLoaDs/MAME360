/**
	* \file		InputDevice.h
  *         Virtual base class providing input device functions
	*/

#pragma once

//= I N C L U D E S ===========================================================
#include "xbox_Mame.h"
#include "osd_cpu.h"


//= D E F I N E S =============================================================
  // Dummy port index to allow the keyboard to be inserted into any port
#define KEYBOARD_AUTOSEARCH_PORT      0xFF

//= C L A S S E S =============================================================
class CInputManager;

  //! \class    CInputDevice
  //! \brief    Virtual base class providing input device functions
class CInputDevice
{
public:
		//------------------------------------------------------
		//	Constructor
		//------------------------------------------------------
  CInputDevice( void ) :
    m_inputManager( NULL ),
    m_portIndex( 0 ),
    m_portMask( 0 ), 
    m_portName( 0 ),
    m_deviceHandle( NULL )
  {
    memset( &m_state, 0, sizeof(m_state) );
    memset( &m_feedback, 0, sizeof(m_feedback) );
    memset( &m_caps, 0, sizeof(m_caps) );
  }

 
  virtual BOOL Create( UINT32 gpIndex, CInputManager *inputManager ) {
    if( inputManager )
    {
      m_inputManager = inputManager;
      m_portIndex = gpIndex; 

      return TRUE;
    }
    return FALSE;
  }

 
  virtual void PollDevice( void ) = 0; 
  virtual void AttachRemoveDevices( void ) = 0; 
  virtual BOOL IsConnected( void ) const {
    return m_deviceHandle >= (HANDLE)0x00;
  }

		//------------------------------------------------------
		//	WaitForAnyInput
		//! \brief		Wait for anything to be pressed/moved on
		//!            the device
		//------------------------------------------------------
	virtual void WaitForAnyInput( void ) = 0;

		//------------------------------------------------------
		//	WaitForNoInput
		//! \brief		Wait for everything to be released on the
		//!            device
		//------------------------------------------------------
  virtual void WaitForNoInput( void ) = 0;

		//------------------------------------------------------
		//	SetDeviceFeedbackState
    //! Send a force feedback effect to a device
    //!
    //! \param  feedback - Struct describing the effect to send
		//------------------------------------------------------
	inline BOOL SetDeviceFeedbackState( const XINPUT_FF_EFFECT &feedback ) {
		 
		return TRUE;
	}

		//------------------------------------------------------
		//	GetDeviceCaps
    //! Return the capabilities of this device
    //!
    //! \retval   const XINPUT_CAPABILITIES * - The requested
    //!                                         caps object
		//------------------------------------------------------
  const XINPUT_CAPABILITIES *GetDeviceCaps( void ) const {
	  if( IsConnected() )
      return &m_caps;

    return NULL;
  }

	HANDLE					    m_deviceHandle;	          //!<	Input handle for this device
	XINPUT_STATE		   m_state;	       //!<	device device state struct
  protected:

  CInputManager       *m_inputManager;

  DWORD               m_portIndex;              //!<  Port this device is connected to
  DWORD               m_portMask, m_portName;


  
  XINPUT_FF_EFFECT     m_feedback;	   //!<	Feedback struct
  XINPUT_CAPABILITIES  m_caps;         //!<  device device capabilities
};

