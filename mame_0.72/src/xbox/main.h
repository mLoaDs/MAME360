#include <xtl.h>
#include <xui.h>
#include <xuiapp.h>
#include "xbox_Mame.h"

#ifndef MAIN_H
#define MAIN_H
 
extern HRESULT RenderGame( IDirect3DDevice9 *pDevice );

class CMameLauncherApp : public CXuiModule
{
protected:
    // Override RegisterXuiClasses so that CMyApp can register classes.
    virtual HRESULT RegisterXuiClasses();

    // Override UnregisterXuiClasses so that CMyApp can unregister classes. 
    virtual HRESULT UnregisterXuiClasses();
 	
};

#endif