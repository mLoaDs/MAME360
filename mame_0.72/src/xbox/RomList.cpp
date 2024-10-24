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

extern "C" {
#include "osd_cpu.h"
#include "driver.h"
#include "mame.h"
}

using namespace std;

extern const struct GameDriver *drivers[];


extern void DrawDriverProgressData( const char *fileName, DWORD index, DWORD total );
 
char szRoms[MAX_PATH]; 
char szRomPath[MAX_PATH];
 
extern IDirect3DDevice9 *pDevice;
extern CMameLauncherApp app;
extern char pshader[255];

char InGamePreview[MAX_PATH];

int nLastRom = 0;
int nLastFilter = 0;
int HideChildren = 0;
int ThreeOrFourPlayerOnly = 0;

wchar_t DeviceText[60];
extern bool exitGame;

std::vector<std::string> m_ListData;
 
std::map<std::string, int> mapType;
std::map<std::string, int> mapRoms;

std::string ReplaceCharInString(const std::string & source, char charToReplace, const std::string replaceString);

void UpdateConsole(char *text);

static int CurrentFilter = 0;
char rom_filename[256];


wchar_t ucString[42];

HXUIOBJ hRomListScene;
HXUIOBJ hOptionsScene = NULL;

LPCWSTR MultiCharToUniChar(char* mbString)
{
	int len = strlen(mbString) + 1;	
	mbstowcs(ucString, mbString, len);
	return (LPCWSTR)ucString;
}

const char *GetFilename (char *path, const char *InFileName, char *fext)
{
    static char filename [MAX_PATH + 1];
    char dir [_MAX_DIR + 1];
    char drive [_MAX_DRIVE + 1];
    char fname [42];
    char ext [_MAX_EXT + 1];
   _splitpath (InFileName, drive, dir, fname, ext);

   std::string fatxfname(fname);
	if (fatxfname.length() > 37)
	{
			fatxfname = fatxfname.substr(0,36);
	}

   _snprintf(filename, sizeof(filename),  "%s\\%s.%s",path, 
              fatxfname.c_str(), fext);
    return (filename);
}

void InRescanRomsFirstFunc(XUIMessage *pMsg, InRescanRomsStruct* pData, char *szPath)
{
    XuiMessage(pMsg,XM_MESSAGE_ON_RESCAN_ROMS);
    _XuiMessageExtra(pMsg,(XUIMessageData*) pData, sizeof(*pData));

	
}


// Handler for the XM_NOTIFY message
HRESULT CRomListScene::OnNotifyPress( HXUIOBJ hObjPressed, 
       BOOL& bHandled )
    {
 
		char rom[255];
		wchar_t LabelText[255];
		char shaderName[42];

		int nIndex = 0;
		int drvIdx = 0;

        if ( hObjPressed == m_RomList )
        {
			nIndex = m_RomList.GetCurSel();
				
			LPCWSTR romName = m_RomList.GetText(nIndex);
			 
			sprintf(rom,"%S",romName);

			drvIdx = mapRoms[rom];
		 
			main_mame(drvIdx);
			
			XuiRenderRestoreState(app.GetDC());
			bHandled = TRUE;
			return S_OK;
  
        }
		else if (hObjPressed == m_Options)
		{

			XuiSceneCreate( L"file://game:/media/mame.xzp#Skin\\", L"Options.xur", NULL, &hOptionsScene );
			this->NavigateForward(hOptionsScene);

			bHandled = TRUE;
			return S_OK;
		}
	 
	
        bHandled = TRUE;
        return S_OK;
    }


    //----------------------------------------------------------------------------------
    // Performs initialization tasks - retreives controls.
    //----------------------------------------------------------------------------------
HRESULT CRomListScene::OnInit( XUIMessageInit* pInitData, BOOL& bHandled )
    {

		wchar_t LabelText[255];
		char shaderName[42];
 
        // Retrieve controls for later use.
        GetChildById( L"XuiAddToFavorites", &m_AddToFavorites );
        GetChildById( L"XuiImage1", &m_SkinImage );
        GetChildById( L"XuiMainMenu", &m_Back );		 
		GetChildById( L"XuiRomList", &m_RomList );
		GetChildById( L"XuiRomPreview", &m_PreviewImage );	
		GetChildById( L"XuiRomTitle", &m_TitleImage );			 
		GetChildById( L"XuiBackVideoRoms", &m_BackVideo );
		GetChildById( L"XuiCurrentDeviceText", &m_DeviceText);
		GetChildById( L"XuiOptions", &m_ConsoleOutput);

		GetChildById( L"XuiGameInfo", &m_GameInfo);
		GetChildById( L"XuiRomName", &m_RomName);
		GetChildById( L"XuiROMInfo", &m_RomInfo);
		GetChildById( L"XuiReleasedBy", &m_ReleasedBy);
		GetChildById( L"XuiGenre", &m_Genre);

		GetChildById( L"XuiVersion", &m_Version);


		GetChildById( L"XuiButtonOptions", &m_Options);
		GetChildById( L"XuiShaderName", &m_ShaderName);
		
 
		 

		//m_RomList.DiscardResources(XUI_DISCARD_ALL);
		m_RomList.SetFocus();
		m_RomList.SetCurSel(0);
 
		swprintf_s(DeviceText, L"Current Hardware : All Games", szRomPath);
		m_DeviceText.SetText(DeviceText);
 
		hRomListScene = this->m_hObj;
 
		swprintf_s(LabelText,L"%S", LVERSION_STRING);

		m_ListData.clear();
		m_Version.SetText(LabelText);		 

		CurrentFilter = nLastFilter;
		XuiImageElementSetImagePath(m_PreviewImage.m_hObj, L"");

		XUIMessage xuiMsg;
		InRescanRomsStruct msgData;
		InRescanRomsFirstFunc( &xuiMsg, &msgData, "GAME:\\ROMS\\");
		XuiSendMessage( m_RomList.m_hObj, &xuiMsg );
 
		
		_splitpath(pshader,NULL,NULL,shaderName,NULL);
		swprintf_s(LabelText,L"%S", shaderName);
		m_ShaderName.SetText(LabelText);

		m_RomList.SetShow(true);
		m_RomList.SetFocus();		
		m_RomList.SetCurSel(nLastRom);
		m_RomList.SetCurSelVisible(nLastRom);

		bHandled = TRUE;
        return S_OK;
    }

CRomList::CRomList()
{
  
}
int CRomList::InitRomList()
{

	RomCount = 0;
	RomListOK = false;

	m_vecAvailRomList.clear();	 

	return 0;
} 

int CRomList::FreeRomList()
{
	m_vecAvailRomList.clear();
	 
	return 0;
}

int CRomList::AvRoms()
{

	RomCount = m_vecAvailRomList.size();
	 
	return 0;
}

HRESULT CRomList::OnNotify( XUINotify *hObj, BOOL& bHandled )
{	
	wchar_t previewPath[MAX_PATH];
	wchar_t titlePath[MAX_PATH];
	wchar_t InfoText[255];

	char PreviewFName[MAX_PATH];
	char TitleFName[MAX_PATH];

	char rom[255];

	HXUIOBJ hTitleImage;

	HXUIOBJ hGameInfoText;
	HXUIOBJ hRomNameText;
	HXUIOBJ hRomInfoText;
	HXUIOBJ hReleasedText;
	HXUIOBJ hGenreText;
	HXUIOBJ hParentText;

	int nIndex = 0;
	int drvIdx = 0;
	switch(hObj->dwNotify)
	{
		case XN_SELCHANGED:			 						 
			nIndex = XuiListGetCurSel( this->m_hObj, NULL ); 
			LPCWSTR romName = XuiListGetText(this->m_hObj, nIndex);
			 
			sprintf(rom,"%S",romName);

			drvIdx = mapRoms[rom];

			strcpy(TitleFName, GetFilename((char *)g_FileIOConfig.m_screenshotPath.c_str(),  drivers[drvIdx]->name , "png"));			 
 
			XuiElementGetChildById( hRomListScene, 
                L"XuiRomTitle", &hTitleImage );

			XuiElementGetChildById( hRomListScene, 
                L"XuiGameInfo", &hGameInfoText );

			XuiElementGetChildById( hRomListScene, 
                L"XuiRomName", &hRomNameText );

			XuiElementGetChildById( hRomListScene, 
                L"XuiROMInfo", &hRomInfoText );

			XuiElementGetChildById( hRomListScene, 
                L"XuiReleasedBy", &hReleasedText );


			XuiElementGetChildById( hRomListScene, 
                L"XuiGenre", &hGenreText );

			XuiElementGetChildById( hRomListScene, 
                L"XuiParent", &hParentText );

			swprintf_s(InfoText,L"%S", drivers[drvIdx]->manufacturer );
			XuiTextElementSetText(hReleasedText,InfoText);
 
			swprintf_s(InfoText,L"%S", drivers[drvIdx]->description );
			XuiTextElementSetText(hRomNameText,InfoText);
 
			swprintf_s(InfoText,L"%S", drivers[drvIdx]->year );
			XuiTextElementSetText(hGameInfoText,InfoText);

			swprintf_s(InfoText,L"%S", drivers[drvIdx]->name );
			XuiTextElementSetText(hParentText,InfoText);
			
 
			
			if (GetFileAttributes(TitleFName) != -1)
			{
				string titleName(TitleFName);

				titleName = ReplaceCharInString(titleName, '\\',"/");
				swprintf_s(titlePath,L"file://%S", titleName.c_str());
				XuiImageElementSetImagePath(hTitleImage, titlePath);


				swprintf_s(titlePath,L"file://%S", ReplaceCharInString(g_FileIOConfig.m_screenshotPath, '\\', "/").c_str());
				//XuiElementDiscardResources(hTitleImage, XUI_DISCARD_ALL);
				XuiElementSetBasePath(hTitleImage, titlePath);
 
			}
			else
			{		
				XuiImageElementSetImagePath(hTitleImage, L"no_title.png");
			}
	}

	return S_OK;

}
 

HRESULT CRomList::OnRescanRoms( char *szPath,  BOOL& bHandled )
{ 
	HXUIOBJ hRomCountText; 
	wchar_t RomCountText[60];

	bool IsFiltered = false;
	std::vector<std::string> vecTempRomList;
	std::vector<std::string> vecAvailRomListFileName;
	std::vector<std::string> vecAvailRomList;
	std::vector<int>		 vecAvailRomIndex;

	InitRomList();

	DeleteItems(0, m_vecAvailRomList.size());
  
	m_vecAvailRomList.clear();

	DrawDriverProgressData("",0,0);

	if (m_ListData.empty())
	{

		for (int d = 0; d < 4; d++) 
		{

			HANDLE hFind;	
			WIN32_FIND_DATAA oFindData;			 

			switch (d)
			{
				case 0:
					strcpy((char *)szRoms,  g_FileIOConfig.m_romPath0.c_str() );
					break;
				case 1:
					strcpy((char *)szRoms,  g_FileIOConfig.m_romPath1.c_str() );
					break;
				case 2:
					strcpy((char *)szRoms,  g_FileIOConfig.m_romPath2.c_str() );
					break;
				case 3:
					strcpy((char *)szRoms,  g_FileIOConfig.m_romPath3.c_str() );
					break;
			}
			
			strcat(szRoms, "\\*.*");

			hFind = FindFirstFile(szRoms, &oFindData);

			if (hFind != INVALID_HANDLE_VALUE)
			{
				do
				{
					std::vector<std::string>::iterator iter = std::find(m_ListData.begin(), m_ListData.end(), _strlwr(oFindData.cFileName));

					if (iter == m_ListData.end()) {
						m_ListData.push_back(_strlwr(oFindData.cFileName));		
					}
				
				} while (FindNextFile(hFind, &oFindData));	
			}
		}
		  
	}
 
	InitRomList();

	mapType.clear();
 
	// Count and sort the game drivers (Taken from XMame)
    DWORD totalMAMEGames = 0;
    for( totalMAMEGames = 0; drivers[totalMAMEGames]; ++totalMAMEGames)
	{
		char fullname[60];
		sprintf(fullname,"%s.zip",drivers[totalMAMEGames]->name);
		vecAvailRomListFileName.push_back(fullname);
		vecAvailRomList.push_back(drivers[totalMAMEGames]->name);
		vecAvailRomIndex.push_back(totalMAMEGames);

		mapType[fullname] = totalMAMEGames;
		mapRoms[drivers[totalMAMEGames]->description] = totalMAMEGames;

	}

 
 
	for (unsigned int x = 0; x < vecAvailRomListFileName.size(); x++) {
		for (unsigned int y = 0; y < m_ListData.size() ; y++) {
			if (m_ListData[y] == vecAvailRomListFileName[x]) {
 				 
				m_vecAvailRomList.push_back(drivers[x]->description);
				//m_vecAvailRomReleasedBy.push_back(drivers[x]->manufacturer);						 
				//m_vecAvailRomManufacturer.push_back(drivers[x]->year);
							
			}

		}


	}

	std::sort(m_vecAvailRomList.begin(), m_vecAvailRomList.end());


	AvRoms();
	InsertItems( 0, RomCount );
	XuiElementGetChildById( hRomListScene, L"XuiNumberOfRoms", &hRomCountText );	 
	swprintf(RomCountText,L"%d/%d Games Found",RomCount, totalMAMEGames);	 
	XuiTextElementSetText(hRomCountText,RomCountText);

	bHandled = TRUE;	
    return( S_OK );
}

HRESULT CRomList::OnInit(XUIMessageInit *pInitData, BOOL& bHandled)
{
 
	XUIMessage xuiMsg;
	InRescanRomsStruct msgData;
	InRescanRomsFirstFunc( &xuiMsg, &msgData, "GAME:\\ROMS\\" );
	XuiSendMessage( m_hObj, &xuiMsg );

	bHandled = TRUE;
    return S_OK;
}

HRESULT CRomList::OnGetItemCountAll(
        XUIMessageGetItemCount *pGetItemCountData, 
        BOOL& bHandled)
    {
        pGetItemCountData->cItems = RomCount;
        bHandled = TRUE;
        return S_OK;
    }


// Gets called every frame
HRESULT CRomList::OnGetSourceDataText(
    XUIMessageGetSourceText *pGetSourceTextData, 
    BOOL& bHandled)
{
	
    
    if( ( 0 == pGetSourceTextData->iData ) && ( ( pGetSourceTextData->bItemData ) ) ) {

			LPCWSTR lpszwBuffer = MultiCharToUniChar((char *)m_vecAvailRomList[pGetSourceTextData->iItem].c_str());

            pGetSourceTextData->szText = lpszwBuffer;

            bHandled = TRUE;
        }
        return S_OK;

}



std::string ReplaceCharInString(  
    const std::string & source, 
    char charToReplace, 
    const std::string replaceString 
    ) 
{ 
    std::string result; 
 
    // For each character in source string: 
    const char * pch = source.c_str(); 
    while ( *pch != '\0' ) 
    { 
        // Found character to be replaced? 
        if ( *pch == charToReplace ) 
        { 
            result += replaceString; 
        } 
        else 
        { 
            // Just copy original character 
            result += (*pch); 
        } 
 
        // Move to next character 
        ++pch; 
    } 
 
    return result; 
} 

