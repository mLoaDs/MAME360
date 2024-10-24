/**
	* \file		FontSet.h
  *         Simple font manager class
	*/

#pragma once

#ifdef __cplusplus

//= I N C L U D E S ===========================================================
#include "xbox_Mame.h"
#include "osd_cpu.h"
#include "atgfont.h"

//= D E F I N E S =============================================================

#define CREATEFONT( fntObj, fntName )     fntObj.Create( fntName )
#define DESTROYFONT(fntObj) fntObj.Destroy()

#define DEFAULT_FONT      "GAME:\\Media\\Courier_New_11.xpr"
#define FIXEDWIDTH_FONT   "GAME:\\Media\\Courier_New_11.xpr"
#define SMALLTHIN_FONT    "GAME:\\Media\\Courier_New_11.xpr"
#define LARGETHIN_FONT    "GAME:\\Media\\Courier_New_11.xpr"


//= C L A S S E S =============================================================

  //! \class    CFontSet
  //! \brief    Font manager class
class CFontSet
{
public:

    //-------------------------------------------------------------------
	  //	Create
	  //! \brief		Creates the font objects
    //! \return   BOOL - pass or failure
    //-------------------------------------------------------------------
  BOOL Create( void ) {
   		// Load the fonts, reboot if any are missing (without a font,
      //  no error message is possible (as of today)
    if( FAILED( CREATEFONT( m_fonts[0], DEFAULT_FONT ) ) )
    {
      PRINTMSG_TO_LOG(( T_ERROR, "Failed loading font Media/" DEFAULT_FONT "!" ));
      return FALSE;
    }

    if( FAILED( CREATEFONT( m_fonts[1], FIXEDWIDTH_FONT ) ) )
    {
      PRINTMSG_TO_LOG(( T_ERROR, "Failed loading font Media/" FIXEDWIDTH_FONT "!" ));
      return FALSE;
    }

    if( FAILED( CREATEFONT( m_fonts[2], SMALLTHIN_FONT ) ) )
    {
      PRINTMSG_TO_LOG(( T_ERROR, "Failed loading font Media/" SMALLTHIN_FONT "!" ));
      return FALSE;
    }

    if( FAILED( CREATEFONT( m_fonts[3], LARGETHIN_FONT ) ) )
    {
      PRINTMSG_TO_LOG(( T_ERROR, "Failed loading font Media/" LARGETHIN_FONT "!" ));
      return FALSE;
    }

    return TRUE;
  }

  void Destroy()
  {
		DESTROYFONT(m_fonts[0]);
		DESTROYFONT(m_fonts[1]);
		DESTROYFONT(m_fonts[2]);
		DESTROYFONT(m_fonts[3]);

  }

  ATG::Font &GetFont( fonttype fType ) { assert(fType < 4); return m_fonts[fType]; }

  ATG::Font &DefaultFont( void ) { return m_fonts[0]; }
  ATG::Font &FixedWidthFont( void ) { return m_fonts[1]; }
  ATG::Font &SmallThinFont( void ) { return m_fonts[2]; }
  ATG::Font &LargeThinFont( void ) { return m_fonts[3]; }


  FLOAT DefaultFontHeight( void ) const { return GetFontHeight( 0 ); }
  FLOAT FixedWidthFontHeight( void ) const { return GetFontHeight( 1 ); }
  FLOAT SmallThinFontHeight( void ) const { return GetFontHeight( 2 ); }
  FLOAT LargeThinFontHeight( void ) const { return GetFontHeight( 3 ); }

protected:
  FLOAT GetFontHeight( UINT32 index ) const {
    FLOAT textWidth, textHeight; 
    m_fonts[index].GetTextExtent( L"kg^@i!", &textWidth, &textHeight );
    return textHeight;
  }

  ATG::Font     m_fonts[4];
};

#endif    // ifdef __cplusplus


