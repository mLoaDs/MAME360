/**
	* \file		GraphicsManager.h
	*
	*/

#ifndef _GRAPHICSMANAGER_H__
#define _GRAPHICSMANAGER_H__


//= I N C L U D E S ===========================================================
#include "xbox_Mame.h"
#include <AtgApp.h>


extern IDirect3DDevice9 *pDevice;

//= C L A S S E S =============================================================
class CGraphicsManager
{
public:
		//------------------------------------------------------
		//	Constructor
		//------------------------------------------------------
	CGraphicsManager( void ) {
		memset( this, NULL, sizeof(*this) );
	}


		//------------------------------------------------------
		//	Create
		//------------------------------------------------------
	BOOL Create( BOOL enableVSYNC, BOOL smallFootprint ) {
		if( m_created )
			return FALSE;


		m_pD3DDevice = pDevice;


		return TRUE;
	}

		//------------------------------------------------------
		//	GetD3DDevice
		//------------------------------------------------------
	LPDIRECT3DDEVICE9 GetD3DDevice( void ) { return m_pD3DDevice; }

protected:
	static BOOL								m_created;					//!<	Whether or not this singleton has been created
	LPDIRECT3D9								m_pD3D;							//!<	Main Direct3D interface
	LPDIRECT3DDEVICE9					m_pD3DDevice;				//!<	The rendering device

	UINT32										m_backBufferWidth;		//!<	The width of the screen
	UINT32										m_backBufferHeight;		//!<	The height of the screen
};

#endif

