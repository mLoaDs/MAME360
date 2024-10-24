#include <xtl.h>
#include <xmedia2.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "xbox_Mame.h"
#include "xbox_FileIO.h"
#include "main.h"
#include "RomList.h"
#include "Options.h"
#include "xbox_Direct3DRenderer.h"

extern "C" {
#include "osd_cpu.h"
#include "driver.h"
#include "mame.h"
}

using namespace std;

extern const struct GameDriver *drivers[];
extern char			pshader[255];
extern IDirect3DDevice9 *pDevice;
extern CMameLauncherApp app;
extern BOOL g_soundEnabled;
 
extern bool exitGame;

std::vector<std::string> m_ShaderListData;
 
static int CurrentFilter = 0;

HXUIOBJ hShaderListScene;
 
void InRescanShadersFirstFunc(XUIMessage *pMsg, InRescanShadersStruct* pData, char *szPath)
{
    XuiMessage(pMsg,XM_MESSAGE_ON_RESCAN_SHADERS);
    _XuiMessageExtra(pMsg,(XUIMessageData*) pData, sizeof(*pData));	
}


// Handler for the XM_NOTIFY message
HRESULT CShaderListScene::OnNotifyPress( HXUIOBJ hObjPressed, 
       BOOL& bHandled )
    {
 
		int nIndex = 0;
		int drvIdx = 0;
		

        if ( hObjPressed == m_ShaderList )
        {
			nIndex = m_ShaderList.GetCurSel();
				
			LPCWSTR shaderName = m_ShaderList.GetText(nIndex);
			 
			sprintf(pshader,"GAME:\\Shaders\\%S",shaderName);			 					
			m_ShaderName.SetText(shaderName);
			g_FileIOConfig.m_shaderFileName = pshader;
			bHandled = TRUE;
			return S_OK;
  
        }
		else if (hObjPressed == m_Back)
		{
			SaveOptions();
			this->NavigateBack();
			bHandled = TRUE;
			return S_OK;
		}
		else if (hObjPressed == m_EnableSound)
		{
			if (m_EnableSound.IsChecked())
			{
				g_soundEnabled = 1;
			}
			else
			{
				g_soundEnabled = 0;
			}
			bHandled = TRUE;
			return S_OK;
		}
		else if (hObjPressed == m_Cheats)
		{
			if (m_Cheats.IsChecked())
			{
				options.cheat = 1;
			}
			else
			{
				options.cheat = 0;
			}
			bHandled = TRUE;
			return S_OK;
		}
		else if (hObjPressed == m_Vsync)
		{
			if (m_Vsync.IsChecked())
			{
				g_rendererOptions.m_vsync = 1;
			}
			else
			{
				g_rendererOptions.m_vsync = 0;
			}
			bHandled = TRUE;
			return S_OK;
		}
		else if (hObjPressed == m_Aspect)
		{
			if (m_Aspect.IsChecked())
			{
				g_rendererOptions.m_preserveAspectRatio = 1;
			}
			else
			{
				g_rendererOptions.m_preserveAspectRatio = 0;
			}
			bHandled = TRUE;
			return S_OK;
		}
		else if (hObjPressed == m_Throttle)
		{
			if (m_Throttle.IsChecked())
			{
				g_rendererOptions.m_throttleFramerate = 1;
			}
			else
			{
				g_rendererOptions.m_throttleFramerate = 0;
			}
			bHandled = TRUE;
			return S_OK;
		}
	
        bHandled = TRUE;
        return S_OK;
    }


    //----------------------------------------------------------------------------------
    // Performs initialization tasks - retreives controls.
    //----------------------------------------------------------------------------------
HRESULT CShaderListScene::OnInit( XUIMessageInit* pInitData, BOOL& bHandled )
    {

		wchar_t LabelText[60];
		char shaderName[42];

        // Retrieve controls for later use.
        GetChildById( L"XuiButtonBack", &m_Back );		 
		GetChildById( L"XuiCheckboxSoundEnable", &m_EnableSound ); 
		GetChildById( L"XuiShaderList", &m_ShaderList);
		GetChildById( L"XuiShaderName", &m_ShaderName);
		GetChildById( L"XuiCheckboxCheats", &m_Cheats);
		GetChildById( L"XuiCheckboxVSYNC", &m_Vsync);
		GetChildById( L"XuiCheckboxThrottle", &m_Throttle);
		GetChildById( L"XuiCheckboxAspect", &m_Aspect);
		
 
		hShaderListScene = this->m_hObj;
 
		m_ShaderListData.clear(); 

		XUIMessage xuiMsg;
		InRescanShadersStruct msgData;
		InRescanShadersFirstFunc( &xuiMsg, &msgData, "GAME:\\Shaders\\");
		XuiSendMessage( m_ShaderList.m_hObj, &xuiMsg );
 
 	
		_splitpath(pshader,NULL,NULL,shaderName,NULL);
		swprintf_s(LabelText,L"%S", shaderName);
		m_ShaderName.SetText(LabelText);

		m_EnableSound.SetFocus();

		if (g_soundEnabled == 1)
		{
			m_EnableSound.SetCheck(true);
		}
		else
		{
			m_EnableSound.SetCheck(false);
		}

		if (options.cheat == 1)
		{
			m_Cheats.SetCheck(true);
		}
		else
		{
			m_Cheats.SetCheck(false);
		}

		if (g_rendererOptions.m_vsync == 1)
		{
			m_Vsync.SetCheck(true);
		}
		else
		{
			m_Vsync.SetCheck(false);
		}

		if (g_rendererOptions.m_preserveAspectRatio == 1)
		{
			m_Aspect.SetCheck(true);
		}
		else
		{
			m_Aspect.SetCheck(false);
		}

		if (g_rendererOptions.m_throttleFramerate == 1)
		{
			m_Throttle.SetCheck(true);
		}
		else
		{
			m_Throttle.SetCheck(false);
		}



		bHandled = TRUE;
        return S_OK;
    }

CShadersList::CShadersList()
{
  
}
int CShadersList::InitShaderList()
{

	ShaderCount = 0;
	ShaderListOK = false;
	
	m_vecAvailShaderList.clear();	 

	return 0;
} 

int CShadersList::FreeShaderList()
{
	m_vecAvailShaderList.clear();
	 
	return 0;
}

int CShadersList::AvShaders()
{

	ShaderCount = m_vecAvailShaderList.size();
	 
	return 0;
}

HRESULT CShadersList::OnNotify( XUINotify *hObj, BOOL& bHandled )
{	


	return S_OK;

}
 

HRESULT CShadersList::OnRescanShaders( char *szPath,  BOOL& bHandled )
{ 
 
	char szShaderName[255];

	InitShaderList();

	DeleteItems(0, m_vecAvailShaderList.size());
  
	m_vecAvailShaderList.clear();

 
	if (m_ShaderListData.empty())
	{

 
			HANDLE hFind;	
			WIN32_FIND_DATAA oFindData;			 
 
			strcpy(szShaderName, "GAME:\\Shaders\\*.fx");

			hFind = FindFirstFile(szShaderName, &oFindData);

			if (hFind != INVALID_HANDLE_VALUE)
			{
				do
				{
					m_ShaderListData.push_back(_strlwr(oFindData.cFileName));	
					m_vecAvailShaderList.push_back((oFindData.cFileName));
					 				
				} while (FindNextFile(hFind, &oFindData));	
			}
		 
		  
	}
 	 
	std::sort(m_vecAvailShaderList.begin(), m_vecAvailShaderList.end());


	AvShaders();
	InsertItems( 0, ShaderCount );

	bHandled = TRUE;	
    return( S_OK );
}

HRESULT CShadersList::OnInit(XUIMessageInit *pInitData, BOOL& bHandled)
{
 
	XUIMessage xuiMsg;
	InRescanShadersStruct msgData;
	InRescanShadersFirstFunc( &xuiMsg, &msgData, "GAME:\\Shaders\\" );
	XuiSendMessage( m_hObj, &xuiMsg );

	bHandled = TRUE;
    return S_OK;
}

HRESULT CShadersList::OnGetItemCountAll(
        XUIMessageGetItemCount *pGetItemCountData, 
        BOOL& bHandled)
    {
        pGetItemCountData->cItems = ShaderCount;
        bHandled = TRUE;
        return S_OK;
    }


// Gets called every frame
HRESULT CShadersList::OnGetSourceDataText(
    XUIMessageGetSourceText *pGetSourceTextData, 
    BOOL& bHandled)
{
	
    
    if( ( 0 == pGetSourceTextData->iData ) && ( ( pGetSourceTextData->bItemData ) ) ) {

			LPCWSTR lpszwBuffer = MultiCharToUniChar((char *)m_vecAvailShaderList[pGetSourceTextData->iItem].c_str());

            pGetSourceTextData->szText = lpszwBuffer;

            bHandled = TRUE;
        }
        return S_OK;

}

 