#include <stdarg.h>
#include <stdio.h>
#include <crtdbg.h>

#include "xbox_Mame.h"
#include "main.h"
#include "RomList.h"
#include "Options.h"
#include "InputManager.h"
#include "GraphicsManager.h"
#include "DebugLogger.h"
#include "FontSet.h"
#include "xbox_Direct3DRenderer.h"


// Global XUI Stuff
CMameLauncherApp app;
IDirect3DDevice9 *pDevice;
IDirect3D9 *pD3D;
D3DPRESENT_PARAMETERS d3dpp;
int Mounted[20];

extern CInputManager	  g_inputManager;
extern CGraphicsManager	  g_graphicsManager;
extern CFontSet           g_fontSet;
extern char				  pshader[255]; 
extern const char *GetFilename (char *path, const char *InFileName, char *fext);

HXUIOBJ hMainScene;

HRESULT CMameLauncherApp::RegisterXuiClasses()
{	
    // We must register the video control classes
	XuiVideoRegister();
	XuiHtmlRegister();  
	XuiSoundXAudioRegister();

	CRomListScene::Register();
	CRomList::Register();	
	CShaderListScene::Register();
	CShadersList::Register();
 
    // Register any other classes necessary for the app/scene
    return S_OK;
}


//--------------------------------------------------------------------------------------
// Name: UnregisterXuiClasses
// Desc: Unregisters all the scene classes.
//--------------------------------------------------------------------------------------
HRESULT CMameLauncherApp::UnregisterXuiClasses()
{
	CRomListScene::Unregister();
	CRomList::Unregister();
	CShaderListScene::Unregister();
	CShadersList::Unregister();
    return S_OK;
}

HRESULT RenderGame( IDirect3DDevice9 *pDevice )
	{
    // Render game graphics.
    pDevice->Clear(
        0,
        NULL,
        D3DCLEAR_TARGET | D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER,
        D3DCOLOR_ARGB( 255, 0, 0, 0 ),
        1.0,
        0 );

    return S_OK;
}




HRESULT InitD3D( IDirect3DDevice9 **ppDevice, 
    D3DPRESENT_PARAMETERS *pd3dPP )
{
    
	// Get the user video settings.
	XVIDEO_MODE VideoMode; 
	XMemSet( &VideoMode, 0, sizeof(XVIDEO_MODE) ); 
	XGetVideoMode( &VideoMode );

    pD3D = Direct3DCreate9( D3D_SDK_VERSION );

	

	D3DPRESENT_PARAMETERS m_d3dpp =
	{
		1280,                // BackBufferWidth;
		720,                // BackBufferHeight;
		D3DFMT_A8R8G8B8,    // BackBufferFormat;
		1,                  // BackBufferCount;
		D3DMULTISAMPLE_NONE,// MultiSampleType;
		0,                  // MultiSampleQuality;
		D3DSWAPEFFECT_DISCARD, // SwapEffect;
		NULL,               // hDeviceWindow;
		FALSE,              // Windowed;
		TRUE,               // EnableAutoDepthStencil;
		D3DFMT_D24S8,       // AutoDepthStencilFormat;
		0,                  // Flags;
		0,                  // FullScreen_RefreshRateInHz;
		D3DPRESENT_INTERVAL_ONE, // FullScreen_PresentationInterval;
	};

 
	if (g_rendererOptions.m_vsync == TRUE)
	{
		m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	}
	else
	{
		m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	}

	if (!VideoMode.fIsWideScreen)
	{
		m_d3dpp.Flags |=  D3DPRESENTFLAG_NO_LETTERBOX;
	}
 



 
    // Create the device.
    return pD3D->CreateDevice(
                    0, 
                    D3DDEVTYPE_HAL,
                    NULL,
                    D3DCREATE_FPU_PRESERVE,
                    &m_d3dpp,
                    ppDevice );
}

int DriveMounted(std::string path)
{
	WIN32_FIND_DATA findFileData;
	memset(&findFileData,0,sizeof(WIN32_FIND_DATA));
	std::string searchcmd = path + "\\*.*";
	HANDLE hFind = FindFirstFile(searchcmd.c_str(), &findFileData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	FindClose(hFind);

	return 1;
}


HRESULT Mount( int Device, char* MountPoint )
{
	char MountConv[260];
	sprintf_s( MountConv,"\\??\\%s", MountPoint );

	char * SysPath = NULL;
	switch( Device )
	{
		case DEVICE_MEMORY_UNIT0:
			SysPath = "\\Device\\Mu0";
			break;
		case DEVICE_MEMORY_UNIT1:
			SysPath = "\\Device\\Mu1";
			break;
		case DEVICE_MEMORY_ONBOARD:
			SysPath = "\\Device\\BuiltInMuSfc";
			break;
		case DEVICE_CDROM0:
			SysPath = "\\Device\\Cdrom0";
			break;
		case DEVICE_HARDISK0_PART1:
			SysPath = "\\Device\\Harddisk0\\Partition1";
			break;
		case DEVICE_HARDISK0_SYSPART:
			SysPath = "\\Device\\Harddisk0\\SystemPartition";
			break;
		case DEVICE_USB0:
			SysPath = "\\Device\\Mass0";
			break;
		case DEVICE_USB1:
			SysPath = "\\Device\\Mass1";
			break;
		case DEVICE_USB2:
			SysPath = "\\Device\\Mass2";
			break;
		case DEVICE_CACHE:
			SysPath = "\\Device\\Harddisk0\\Cache0";
			break;

	}

	STRING sSysPath = { (USHORT)strlen( SysPath ), (USHORT)strlen( SysPath ) + 1, SysPath };
	STRING sMountConv = { (USHORT)strlen( MountConv ), (USHORT)strlen( MountConv ) + 1, MountConv };
	int res = ObCreateSymbolicLink( &sMountConv, &sSysPath );

	if (res != 0)
		return res;

	return DriveMounted(MountPoint);
}


void MountAll()
{
	memset(&Mounted,0,20);

 	Mounted[DEVICE_USB0] = Mount(DEVICE_USB0,"Usb0:");
 	Mounted[DEVICE_USB1] = Mount(DEVICE_USB1,"Usb1:");
 	Mounted[DEVICE_USB2] = Mount(DEVICE_USB2,"Usb2:");
 	Mounted[DEVICE_HARDISK0_PART1] = Mount(DEVICE_HARDISK0_PART1,"Hdd1:");
 	Mounted[DEVICE_HARDISK0_SYSPART] = Mount(DEVICE_HARDISK0_SYSPART,"HddX:");
 	Mounted[DEVICE_MEMORY_UNIT0] = Mount(DEVICE_MEMORY_UNIT0,"Memunit0:");
 	Mounted[DEVICE_MEMORY_UNIT1] = Mount(DEVICE_MEMORY_UNIT1,"Memunit1:");
	Mounted[DEVICE_MEMORY_ONBOARD] = Mount(DEVICE_MEMORY_ONBOARD,"OnBoardMU:"); 
	Mounted[DEVICE_CDROM0] = Mount(DEVICE_CDROM0,"Dvd:"); 
}

// Main program entry point
int WINAPI main()
{
	HRESULT hr;
	DWORD dwPlayerIndex = 0;
	XINPUT_CAPABILITIES Capabilities;
  
	strcpy(pshader,"GAME:\\Shaders\\Default.fx");

	MountAll();
	LoadOptions();	
	XSetFileCacheSize(0x100000);
	XMountUtilityDriveEx(XMOUNTUTILITYDRIVE_FORMAT0,8192, 0);
	 
    // Initialize D3D
    hr = InitD3D( &pDevice, &d3dpp );
 
    // Initialize the application.    
 
    hr = app.InitShared( pDevice, &d3dpp, 
        XuiPNGTextureLoader );
 
    if( FAILED( hr ) )
    { 
        OutputDebugString( "Failed intializing application.\n" );
        return 0;
    }

    // Register a default typeface
    hr = app.RegisterDefaultTypeface( L"Arial Unicode MS", L"file://game:/media/mame.ttf" );
	
    if( FAILED( hr ) )
    {
        OutputDebugString( "Failed to register default typeface.\n" );
        return 0;
    }

	// Initialize the graphics subsystem
    g_graphicsManager.Create( TRUE, FALSE );
	LPDIRECT3DDEVICE9 pD3DDevice = g_graphicsManager.GetD3DDevice();
	ATG::g_pd3dDevice = (ATG::D3DDevice*)pD3DDevice;

	g_fontSet.Create();
 
 
    // Load the skin file used for the scene.
	app.LoadSkin( L"file://game:/media/mame.xzp#Skin\\skin.xur" );     	
	XuiSceneCreate( L"file://game:/media/mame.xzp#Skin\\", L"RomList.xur", NULL, &hMainScene );	
	XuiSceneNavigateFirst( app.GetRootObj(), hMainScene, XUSER_INDEX_FOCUS ); 
 
 
    while( TRUE ) { 
 
		if (XInputGetCapabilities(dwPlayerIndex, XINPUT_FLAG_GAMEPAD, 
		&Capabilities) == ERROR_SUCCESS)
		{
			if ((Capabilities.Type == XINPUT_DEVTYPE_GAMEPAD) 
				&& (Capabilities.SubType == XINPUT_DEVSUBTYPE_ARCADE_STICK ))
			{
				 
			}
		}
  
		RenderGame( pDevice );
 
		// Update XUI
		app.RunFrame();
 
		// Render XUI
		hr = app.Render();

		// Update XUI Timers
		hr = XuiTimersRun();
 
		// Present the frame.
		pDevice->Present( NULL, NULL, NULL, NULL );
		
    }
 
	return 0;
}

void UpdateConsole(char *text)
{
	wchar_t OutputText[255];

	swprintf_s(OutputText, L"%S", text);
	HXUIOBJ hLabel;
	XuiElementGetChildById( hMainScene, L"XuiOptions", &hLabel );
	XuiTextElementSetText( hLabel, OutputText );
	RenderGame( pDevice );
	// Update XUI
	app.RunFrame();
	// Render XUI
	app.Render();
	// Update XUI Timers
	XuiTimersRun();
	// Present the frame.
	pDevice->Present( NULL, NULL, NULL, NULL );
}

