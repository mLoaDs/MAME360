//--------------------------------------------------------------------------------------
// XuiTutorial.cpp
//
// Shows how to display and use a simple XUI scene.
//
// Xbox Advanced Technology Group.
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include <xtl.h>
#include <xui.h>
#include <xuiapp.h>
#include <algorithm>
#include <new>
#include <iostream>
#include <vector>


#ifndef SHADERLIST_H
#define SHADERLIST_H
 
#define XM_MESSAGE_ON_RESCAN_SHADERS  XM_USER

typedef struct
{
    char *szPath;    
}
InRescanShadersStruct;


void InRescanShadersFirstFunc(XUIMessage *pMsg, InRescanShadersStruct* pData, char *szPath);

// Define the message map macro
#define XUI_ON_XM_MESSAGE_ON_RESCAN_SHADERS(MemberFunc)\
    if (pMessage->dwMessage == XM_MESSAGE_ON_RESCAN_SHADERS)\
    {\
        InRescanShadersStruct *pData = (InRescanShadersStruct *) pMessage->pvData;\
        return MemberFunc(pData->szPath,  pMessage->bHandled);\
    }

class CShadersList : CXuiListImpl
{
public:

	XUI_IMPLEMENT_CLASS(CShadersList, L"ShaderList", XUI_CLASS_LIST);

	XUI_BEGIN_MSG_MAP()
		XUI_ON_XM_INIT(OnInit)
		XUI_ON_XM_GET_SOURCE_TEXT(OnGetSourceDataText)
		XUI_ON_XM_GET_ITEMCOUNT_ALL(OnGetItemCountAll)	 
		XUI_ON_XM_NOTIFY( OnNotify )
		XUI_ON_XM_MESSAGE_ON_RESCAN_SHADERS( OnRescanShaders )
	XUI_END_MSG_MAP()

	CShadersList();

	int FreeShaderList();
	int AvShaders();
	int RefreshShaderList();
	int InitShaderList();

	void BuildShaderList();

	int ShaderCount;
	bool ShaderListOK;

	std::vector<std::string> m_vecAvailShaderList;
 
    HRESULT OnInit( XUIMessageInit* pInitData, BOOL& bHandled );
	HRESULT OnNotify( XUINotify *hObj, BOOL& bHandled );
	HRESULT OnGetSourceDataText(XUIMessageGetSourceText *pGetSourceTextData, BOOL& bHandled);
	HRESULT OnGetItemCountAll(XUIMessageGetItemCount *pGetItemCountData, BOOL& bHandled);
	HRESULT OnRescanShaders ( char *szPath, BOOL& bHandled );
	 
};

//--------------------------------------------------------------------------------------
// Scene implementation class.
//--------------------------------------------------------------------------------------
class CShaderListScene : public CXuiSceneImpl
{

protected:

    // Control and Element wrapper objects.
 
	CXuiControl m_Back;  
	CXuiList m_ShaderList;
 
	CXuiCheckbox m_Cheats;
	CXuiCheckbox m_EnableSound;
	CXuiCheckbox m_Vsync;
	CXuiCheckbox m_Aspect;
	CXuiCheckbox m_Throttle;

	CXuiControl m_SelectShader;
	CXuiTextElement m_ShaderName;


    // Message map.
    XUI_BEGIN_MSG_MAP()
		XUI_ON_XM_INIT( OnInit )
		XUI_ON_XM_NOTIFY_PRESS( OnNotifyPress )
    XUI_END_MSG_MAP()

	


 
public:
    HRESULT OnInit( XUIMessageInit* pInitData, BOOL& bHandled );
	HRESULT OnNotifyPress( HXUIOBJ hObjPressed, BOOL& bHandled );
 
public:

    // Define the class. The class name must match the ClassOverride property
    // set for the scene in the UI Authoring tool.
    XUI_IMPLEMENT_CLASS( CShaderListScene, L"ShaderListScene", XUI_CLASS_SCENE )
};


#endif